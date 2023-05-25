#pragma once

#include "auth/OAuth2.hpp"
#include "dto/ApiResponseDTO.hpp"
#include "dto/PetDTO.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/mime/multipart/FileProvider.hpp"
#include "oatpp/web/mime/multipart/InMemoryDataProvider.hpp"
#include "oatpp/web/mime/multipart/PartList.hpp"
#include "oatpp/web/mime/multipart/Reader.hpp"
#include "oatpp/web/protocol/http/outgoing/MultipartBody.hpp"
#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "service/PetService.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class PetController : public oatpp::web::server::api::ApiController
{
  std::shared_ptr<PetService> m_service;

public:
  explicit PetController(OATPP_COMPONENT(std::shared_ptr<PetService>, petService),
                         OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    : oatpp::web::server::api::ApiController(objectMapper), m_service(petService)
  {
  }

  ENDPOINT_INFO(addPet)
  {
    info->summary = "Add a new pet to the store";
    info->addConsumes<Object<PetDTO>>("application/json");
    info->addResponse<Object<PetDTO>>(Status::CODE_200, "application/json");
  };
  ENDPOINT("POST", "/pet", addPet, BODY_DTO(Object<PetDTO>, dto), BUNDLE(String, oauth2UserId),
           BUNDLE(Boolean, oauth2ScopeReadPets), BUNDLE(Boolean, oauth2ScopeWritePets))
  {
    OATPP_LOGD("addPet", "authrizeation: %s: read:pets: %d write:pets: %d", oauth2UserId->c_str(),
               oauth2ScopeReadPets.operator bool(), oauth2ScopeWritePets.operator bool())
    OATPP_ASSERT_HTTP(oauth2ScopeReadPets && oauth2ScopeWritePets, Status::CODE_401, "Unauthorized")
    auto responseDto = m_service->addPet(oauth2UserId, dto);
    return createDtoResponse(Status::CODE_200, responseDto);
    //        auto pets = oatpp::Vector<Object<PetDTO>>::createShared();
  }

  ENDPOINT_INFO(updatePet)
  {
    info->summary = "Update an existing pet";
    //    info->addSecurityRequirement("petstore_auth", {"write:pets", "read:pets"});
    info->addConsumes<Object<PetDTO>>("application/json");
    info->addResponse<Object<PetDTO>>(Status::CODE_200, "successful operation");
    info->addResponse(Status::CODE_400, "Invalid ID supplied");
    info->addResponse(Status::CODE_404, "Pet not found");
    info->addResponse(Status::CODE_405, "Validation exception");
  };
  ENDPOINT("PUT", "/pet", updatePet, BODY_DTO(Object<PetDTO>, dto), BUNDLE(String, oauth2UserId),
           BUNDLE(Boolean, oauth2ScopeReadPets), BUNDLE(Boolean, oauth2ScopeWritePets))
  {
    OATPP_LOGD("updatePet", "")
    auto responseDto = m_service->updatePet(oauth2UserId, dto);
    return createDtoResponse(Status::CODE_200, responseDto);
  };

  ENDPOINT_INFO(findPetsByStatus)
  {
    info->summary = "Finds Pets by status";
    info->addResponse<oatpp::Vector<Object<PetDTO>>>(Status::CODE_200, "application/json");
    info->addResponse<String>(Status::CODE_400, "text/plain");
    info->queryParams.add<String>("status");
  };
  ENDPOINT("GET", "/pet/findByStatus", findPetsByStatus, QUERY(String, status),
           BUNDLE(String, oauth2UserId), BUNDLE(Boolean, oauth2ScopeReadPets),
           BUNDLE(Boolean, oauth2ScopeWritePets))
  {
    OATPP_LOGD("findPetsByStatus", "status=%s", status->c_str())
    auto responseDto = m_service->findPetsByStatus(oauth2UserId, status);
    return createDtoResponse(Status::CODE_200, responseDto);
  }

  ENDPOINT_INFO(findPetsByTags)
  {
    info->summary = "Finds Pets by tags";
    info->description =
        "Multiple tags can be provided with comma separated strings. Use tag1, tag2, tag3 for "
        "testing.";
    info->queryParams.add<String>("tags");
  };
  ENDPOINT("GET", "/pet/findByTags", findPetsByTags, QUERY(String, tags),
           BUNDLE(String, oauth2UserId), BUNDLE(Boolean, oauth2ScopeReadPets),
           BUNDLE(Boolean, oauth2ScopeWritePets))
  {
    OATPP_LOGD("findPetsByTags", "tags=%s", tags->c_str())
    auto responseDto = m_service->findPetsByTags(oauth2UserId, tags);
    return createDtoResponse(Status::CODE_200, responseDto);
  }

  ENDPOINT_INFO(getPetById)
  {
    info->summary = "Find pet by ID";
    info->addResponse<Object<PetDTO>>(Status::CODE_200, "application/json");
    info->addResponse(Status::CODE_400, "Invalid ID supplied");
    info->addResponse(Status::CODE_404, "Pet not found");
    info->pathParams.add<String>("petId");
  };
  ENDPOINT("GET", "/pet/{petId}", getPetById, PATH(Int64, petId), BUNDLE(String, apiKeyUserId))
  {
    OATPP_LOGD("getPetById", "petId=%d apiKeyUserId=%s", petId.getValue(0), apiKeyUserId->c_str())
    //    OATPP_ASSERT_HTTP(authObject->userId != "", Status::CODE_401, "Unauthorized")
    // TODO Add your implementation here.
    auto dto = Object<PetDTO>::createShared();
    return createDtoResponse(Status::CODE_200, dto);
  }

  //  ENDPOINT_INFO(updatePetWithForm)
  //  {
  //    info->summary = "Updates a pet in the store with form data";
  //    info->addResponse(Status::CODE_405, "Invalid input");
  //    info->pathParams.add<String>("petId");
  //    // info->security.append("petstore_auth", {"write:pets", "read:pets"});
  //  };
  //  ENDPOINT("POST", "/pet/{petId}", updatePetWithForm, PATH(Int32, petId),
  // BUNDLE(String, oauth2UserId),
  // BUNDLE(Boolean, oauth2ScopeReadPets), BUNDLE(Boolean, oauth2ScopeWritePets))
  //           BODY_STRING(String, dto))
  //  {
  //    OATPP_LOGD("updatePetWithForm", "petId=%s, dto=%s", petId->c_str(), dto->c_str());
  //    // TODO Add your implementation here.
  //    return createResponse(Status::CODE_200, "OK");
  //  }

  ENDPOINT_INFO(deletePet)
  {
    info->summary = "Deletes a pet";
    info->pathParams.add<Int64>("petId");
  };
  ENDPOINT("DELETE", "/pet/{petId}", deletePet, PATH(Int64, petId), BUNDLE(String, oauth2UserId),
           BUNDLE(Boolean, oauth2ScopeReadPets), BUNDLE(Boolean, oauth2ScopeWritePets))
  {
    OATPP_LOGD("deletePet", "petId=%d", petId.getValue(0))
    // TODO Add your implementation here.
    return createResponse(Status::CODE_200, "OK");
  }

  ENDPOINT_INFO(uploadFile)
  {
    info->summary = "uploads an image";
    info->pathParams.add<Int64>("petId");
    // info->addConsumes<Multipart>("multipart/form-data");
  };
  ENDPOINT("POST", "/pet/{petId}/uploadImage", uploadFile, PATH(Int64, petId, "petId"),
           REQUEST(std::shared_ptr<IncomingRequest>, request), BUNDLE(String, oauth2UserId),
           BUNDLE(Boolean, oauth2ScopeReadPets), BUNDLE(Boolean, oauth2ScopeWritePets))
  {
    OATPP_LOGD("uploadFile", "petId=%d", petId.getValue(0))

    namespace multipart = oatpp::web::mime::multipart;

    /* Prepare multipart container. */
    auto multipartContainer = std::make_shared<multipart::PartList>(request->getHeaders());

    /* Create multipart reader. */
    multipart::Reader multipartReader(multipartContainer.get());

    /* Configure to read part with name "additionalMetadata" into memory */
    multipartReader.setPartReader("additionalMetadata",
                                  multipart::createInMemoryPartReader(256 /* max-data-size */));

    /* Configure to stream part with name "file" to file */
    multipartReader.setPartReader("file", multipart::createFilePartReader("/dev/null"));

    /* Read multipart body */
    request->transferBody(&multipartReader);

    /* Print number of uploaded parts */
    OATPP_LOGD("uploadFile", "parts_count=%d", multipartContainer->count())

    /* Print value of "additionalMetadata" */
    auto additionalMetadata = multipartContainer->getNamedPart("additionalMetadata");
    OATPP_LOGD("Multipart", "additionalMetadata='%s'",
               additionalMetadata->getPayload()->getInMemoryData()->c_str())

    /* Get part by name "file"*/
    auto filePart = multipartContainer->getNamedPart("file");
    auto fileStream = filePart->getPayload()->openInputStream();
    // TODO Process fileStream.

    auto dto = Object<ApiResponseDTO>::createShared();
    return createDtoResponse(Status::CODE_200, dto);
  }

};  // class PetController

#include OATPP_CODEGEN_END(ApiController)
