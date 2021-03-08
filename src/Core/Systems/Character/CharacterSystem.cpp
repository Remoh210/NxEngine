#include "CharacterSystem.h"

#include "Core/Components/TransformComponent/TransformComponent.h"
#include "Core/Components/Physics/RigidBodyComponent.h"
#include "Core/Components/Character/CharacterComponent.h"
#include "Core/Components/Input/InputComponent.h"

#include "Core/Application/Application.h"

namespace ECS
{
	void CharacterSystem::tick(class World *world, float deltaTime)
	{
		world->each<
			TransformComponent,
			RigidBodyComponent,
			InputComponent,
			CharacterComponent>
			([&](
				Entity *ent,
				ComponentHandle<TransformComponent> trantransformComp,
				ComponentHandle<RigidBodyComponent> rigidbodyComp,
				ComponentHandle<InputComponent> inputComp,
				ComponentHandle<CharacterComponent> characterComp) -> void
		{
			if (!Application::GetIsPIE()) { return; }

			//Move Forward
			if (inputComp->GetKeyPressed(InputKey::KEY_W))
			{
				//Values
				
				vec3 curPos = rigidbodyComp->rigidBody->GetPosition();
				curPos += trantransformComp->transform.GetForwardVector() * characterComp->movementSpeed * deltaTime;
				rigidbodyComp->rigidBody->SetPosition(curPos);
			}


			vec3 position = trantransformComp->transform.position.ToVec();
			vec3 camRelativePos = characterComp->cameraRelativePosition.ToVec();


			camera->Position = position +  vec3(2.0f, 34.f, 15.0f)/*characterComp->cameraRelativePosition.ToVec()*/;

			camera->ProcessMouseMovement(inputComp->GetAxis("Horizontal"), inputComp->GetAxis("Vertical"));


			//camera.updateCameraVectors();
			glm::vec3 lookDirection = camera->Position + camera->Front;
			lookDirection = glm::normalize(camera->Front);
			lookDirection.y = 0.0f;
			glm::vec3 worldUP(0.0f, 1.0f, 0.0f);
			glm::mat4 finalOrientation = glm::inverse(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), -lookDirection * 10.0f, worldUP));

			rigidbodyComp->rigidBody->SetMatRotation(finalOrientation);
			//player->m_meshQOrientation = glm::toQuat(finalOrientation);


		});
	}
}