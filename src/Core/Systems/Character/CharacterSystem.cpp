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
			if (!Application::GetIsPIE())
			{
				return;
			}

			vec3 oldVelVec = rigidbodyComp->rigidBody->GetVelocity();
			vec3 velVec = vec3(0.0f);
			//Move Forward
			if (inputComp->GetKeyPressed(InputKey::KEY_W))
			{
				velVec = trantransformComp->transform.GetForwardVector() * characterComp->movementSpeed * 80.f;
			}
			else if (inputComp->GetKeyPressed(InputKey::KEY_S))
			{
				velVec = -trantransformComp->transform.GetForwardVector() * characterComp->movementSpeed * 80.f;
				
			}
			velVec.y = oldVelVec.y;
			rigidbodyComp->rigidBody->SetVelocity(velVec);

			vec3 position = trantransformComp->transform.position.ToVec();
			vec3 camRelativePos = characterComp->cameraRelativePosition.ToVec();


			//camera->Position = position +  vec3(2.0f, 34.f, 15.0f)/*characterComp->cameraRelativePosition.ToVec()*/;

			camera->ProcessMouseMovement(inputComp->GetAxis("Horizontal"), inputComp->GetAxis("Vertical"));


			//camera.updateCameraVectors();
			glm::vec3 lookDirection = camera->Position + camera->Front;
			lookDirection = glm::normalize(camera->Front);
			lookDirection.y = 0.0f;
			glm::vec3 worldUP(0.0f, 1.0f, 0.0f);
			glm::mat4 finalOrientation = glm::inverse(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), -lookDirection * 10.0f, worldUP));


			glm::vec3 TargetPos = position;
			TargetPos.y += 70.0f;
			glm::vec3 posDirection;
			posDirection.x = cos(glm::radians(camera->Yaw)) * cos(glm::radians(camera->Pitch));
			posDirection.y = sin(glm::radians(camera->Pitch));
			posDirection.z = sin(glm::radians(camera->Yaw)) * cos(glm::radians(camera->Pitch));
			posDirection = glm::normalize(posDirection);

			//if (posDirection.y < 0.05f)
			//	posDirection.y = 0.05f;
			
			camera->Position = TargetPos + posDirection * -120.0f;
			//if (camera->Position.y < 5.05f)
			//{
			//	camera->Position.y = 5.05f;
			//}


			rigidbodyComp->rigidBody->SetMatRotation(finalOrientation);
		});
	}
}