#pragma once

#include "sc_store.h"
#include "sc_assemblytree.h"
#include <gason.h>


// This function takes a meshFacesNode and processes all of the points, normals, rgba32s and uvs on the node.
static bool processMeshPosNormRGBaUV(JsonNode* meshFacesNode, std::vector<SC::Store::Point> &points,
                                                              std::vector<SC::Store::Normal> &normals,
                                                              std::vector<SC::Store::RGBA32> &rgba32s,
                                                              std::vector<SC::Store::UV> &uvs){
    try {
        if(meshFacesNode != nullptr){
            for (auto meshElementFaces : meshFacesNode->value) {
                auto faceElementData = meshElementFaces->value.toNode();
                if (strcmp(meshElementFaces->key, "position") == 0){
                    auto facePoint = meshElementFaces->value.toNode();
                    int xyzCount = 0;
                    float pointX = 0.0f;
                    float pointY = 0.0f;
                    bool bListComplete = false;
                    while(!bListComplete){
                        if(xyzCount == 0){
                            pointX = facePoint->value.toNumber();
                            xyzCount = xyzCount + 1;
                        } else if(xyzCount == 1){
                            pointY = facePoint->value.toNumber();
                            xyzCount = xyzCount + 1;
                        } else {
                            float pointZ = facePoint->value.toNumber();
                            points.push_back(SC::Store::Point(pointX, pointY, pointZ));
                            xyzCount = 0;
                        }
                        if(!facePoint->next) bListComplete = true;
                        else facePoint = facePoint->next;
                    }
                } else if(strcmp(faceElementData->key, "normal") == 0){
                    auto faceNormal = meshElementFaces->value.toNode();
                    int xyzCount = 0;
                    float normalX = 0.0f;
                    float normalY = 0.0f;
                    bool bListComplete = false;
                    while(!bListComplete){
                        if(xyzCount == 0){
                            normalX = faceNormal->value.toNumber();
                            xyzCount = xyzCount + 1;
                        } else if(xyzCount == 1){
                            normalY = faceNormal->value.toNumber();
                            xyzCount = xyzCount + 1;
                        } else {
                            float normalZ = faceNormal->value.toNumber();
                            normals.push_back(SC::Store::Normal(normalX, normalY, normalZ));
                            xyzCount = 0;
                        }
                        if(!faceNormal->next) bListComplete = true;
                        else faceNormal = faceNormal->next;
                    }
                } else if(strcmp(faceElementData->key, "rgba") == 0){
                    auto faceRGBA32 = meshElementFaces->value.toNode();
                    int rgbaCount = 0;
                    int redGBA = 0;
                    int rGreenBA = 0;
                    int rgBlueA = 0;
                    bool bListComplete = false;
                    while(!bListComplete){
                        if(rgbaCount == 0){
                            redGBA = faceRGBA32->value.toNumber();
                            rgbaCount = rgbaCount + 1;
                        } else if(rgbaCount == 1){
                            rGreenBA = faceRGBA32->value.toNumber();
                            rgbaCount = rgbaCount + 1;
                        } else if(rgbaCount == 1){
                            rgBlueA = faceRGBA32->value.toNumber();
                            rgbaCount = rgbaCount + 1;
                        } else {
                            float rgbAlpha = faceRGBA32->value.toNumber();
                            rgba32s.push_back(SC::Store::RGBA32(redGBA, rGreenBA, rgBlueA, rgbAlpha));
                            rgbaCount = 0;
                        }
                        if(!faceRGBA32->next) bListComplete = true;
                        else faceRGBA32 = faceRGBA32->next;
                    }
                } else if(strcmp(faceElementData->key, "uv") == 0){
                    auto faceUV = meshElementFaces->value.toNode();
                    bool uvalue = true;
                    float uValue = 0.0f;
                    bool bListComplete = false;
                    while(!bListComplete){
                        if(uvalue){
                            uValue = faceUV->value.toNumber();
                            uvalue = false;
                        } else {
                            float vValue = faceUV->value.toNumber();
                            uvs.push_back(SC::Store::UV(uValue, vValue));
                            uvalue = true;
                        }
                        if(!faceUV->next) bListComplete = true;
                        else faceUV = faceUV->next;
                    }
                }
            }
        }
        return true;
    } catch (std::exception const &e){
        return false;
    }
}