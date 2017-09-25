#include "bodyGenerator.hpp"

extern "C"
btRigidBody* createBoxBody(std::unique_ptr<parameterPack> input){

	vec3 position = *input->search("position")->getVec3();
	vec3 scale    = *input->search("scale")->getVec3();
	quat rotation = *input->search("rotation")->getQuat();
	btScalar mass = btScalar(input->search("mass")->getFloat());


	btCollisionShape* shape = new btBoxShape(scale.toBullet());

	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(rotation.toBullet(), position.toBullet()));
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo bodyCI(mass, motionState, shape, inertia);
	btRigidBody* body = new btRigidBody(bodyCI);

	dynamicsWorld->addRigidBody(body);

	return body;
}

extern "C"
btRigidBody* createPlaneBody(std::unique_ptr<parameterPack> input){

	vec3 position = *input->search("position")->getVec3();
	vec3 face     = *input->search("face")->getVec3();
	quat rotation = *input->search("rotation")->getQuat();
	btScalar mass = btScalar(input->search("mass")->getFloat());


	btCollisionShape* shape = new btStaticPlaneShape(face.toBullet(), 0);

	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(rotation.toBullet(), position.toBullet()));
	btRigidBody::btRigidBodyConstructionInfo bodyCI(mass, motionState, shape, btVector3(0, 0, 0));
	btRigidBody* body = new btRigidBody(bodyCI);

	btScalar friction = btScalar(0.7);
	body->setFriction(friction);

	dynamicsWorld->addRigidBody(body);

	return body;
}

extern "C"
btRigidBody* createConvexHullShapeBody(std::unique_ptr<parameterPack> input){

	vec3 position = *input->search("position")->getVec3();
	vec3 scale    = *input->search("scale")->getVec3();
	quat rotation = *input->search("rotation")->getQuat();
	btScalar mass = btScalar(input->search("mass")->getFloat());
	auto objectDataPtr = input->search("caller")->getEm()->getElementDataPtr();

	std::vector<btVector3> convexHullShapePoints;

	for(auto elem: *objectDataPtr){
		btVector3 co = btVector3(elem->positionX, elem->positionY, elem->positionZ);
		auto itr = std::find(convexHullShapePoints.begin(), convexHullShapePoints.end(), co);
		if( itr == convexHullShapePoints.end() ){
			glm::vec4 target = glm::scale(glm::mat4(1.0f), scale.toGlm()) * glm::vec4(co.x(), co.y(), co.z(), 1);

			convexHullShapePoints.push_back(
					//NOTE: bulletは物体に0.04のマージンを加えるので、その分だけ小さいオブジェクトを作成する。
					btVector3(
						target.x - (0.04*(-1*((signbit(target.x)*2)-1))), 
						target.y - (0.04*(-1*((signbit(target.y)*2)-1))), 
						target.z - (0.04*(-1*((signbit(target.z)*2)-1))))
					);
		}
	}

	btCollisionShape* shape = new btConvexHullShape( &convexHullShapePoints[0][0], convexHullShapePoints.size(), sizeof(btVector3));

	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(rotation.toBullet(), position.toBullet()));
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo bodyCI(mass, motionState, shape, inertia);
	btRigidBody* body = new btRigidBody(bodyCI);

	dynamicsWorld->addRigidBody(body);

	return body;
}
