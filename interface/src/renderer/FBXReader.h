//
//  FBXReader.h
//  interface
//
//  Created by Andrzej Kapolka on 9/18/13.
//  Copyright (c) 2013 High Fidelity, Inc. All rights reserved.
//

#ifndef __interface__FBXReader__
#define __interface__FBXReader__

#include <QMetaType>
#include <QUrl>
#include <QVarLengthArray>
#include <QVariant>
#include <QVector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class FBXNode;

typedef QList<FBXNode> FBXNodeList;

/// The names of the blendshapes expected by Faceshift, terminated with an empty string.
extern const char* FACESHIFT_BLENDSHAPES[];

class Extents {
public:
    /// set minimum and maximum to FLT_MAX and -FLT_MAX respectively
    void reset();

    /// \param point new point to compare against existing limits
    /// compare point to current limits and expand them if necessary to contain point
    void addPoint(const glm::vec3& point);

    /// \param point
    /// \return true if point is within current limits
    bool containsPoint(const glm::vec3& point) const;

    glm::vec3 minimum;
    glm::vec3 maximum;
};

/// A node within an FBX document.
class FBXNode {
public:
    
    QByteArray name;
    QVariantList properties;
    FBXNodeList children;
};

/// A single blendshape extracted from an FBX document.
class FBXBlendshape {
public:
    
    QVector<int> indices;
    QVector<glm::vec3> vertices;
    QVector<glm::vec3> normals;
};

/// A single joint (transformation node) extracted from an FBX document.
class FBXJoint {
public:

    bool isFree;
    QVector<int> freeLineage;
    int parentIndex;
    float distanceToParent;
    float boneRadius;
    glm::vec3 translation;
    glm::mat4 preTransform;
    glm::quat preRotation;
    glm::quat rotation;
    glm::quat postRotation;
    glm::mat4 postTransform;
    glm::mat4 transform;
    glm::vec3 rotationMin;  // radians
    glm::vec3 rotationMax;  // radians
    glm::quat inverseDefaultRotation;
    glm::quat inverseBindRotation;
    glm::mat4 bindTransform;
    QString name;
    glm::vec3 shapePosition;  // in joint frame
    glm::quat shapeRotation;  // in joint frame
    int shapeType;
};


/// A single binding to a joint in an FBX document.
class FBXCluster {
public:
    
    int jointIndex;
    glm::mat4 inverseBindMatrix;
};

/// A single part of a mesh (with the same material).
class FBXMeshPart {
public:
    
    QVector<int> quadIndices;
    QVector<int> triangleIndices;
    
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    float shininess;
    
    QByteArray diffuseFilename;
    QByteArray normalFilename;
};

/// A single mesh (with optional blendshapes) extracted from an FBX document.
class FBXMesh {
public:
    
    QVector<FBXMeshPart> parts;
    
    QVector<glm::vec3> vertices;
    QVector<glm::vec3> normals;
    QVector<glm::vec3> tangents;
    QVector<glm::vec3> colors;
    QVector<glm::vec2> texCoords;
    QVector<glm::vec4> clusterIndices;
    QVector<glm::vec4> clusterWeights;
    
    QVector<FBXCluster> clusters;
    
    bool isEye;
    
    QVector<FBXBlendshape> blendshapes;
    
    float springiness;
    QVector<QPair<int, int> > springEdges;
    QVector<QVarLengthArray<QPair<int, int>, 4> > vertexConnections;
};

/// An attachment to an FBX document.
class FBXAttachment {
public:
    
    int jointIndex;
    QUrl url;
    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;
};

/// A set of meshes extracted from an FBX document.
class FBXGeometry {
public:

    QVector<FBXJoint> joints;
    QHash<QString, int> jointIndices; ///< 1-based, so as to more easily detect missing indices
    
    QVector<FBXMesh> meshes;
    
    glm::mat4 offset;
    
    int leftEyeJointIndex;
    int rightEyeJointIndex;
    int neckJointIndex;
    int rootJointIndex;
    int leanJointIndex;
    int headJointIndex;
    int leftHandJointIndex;
    int rightHandJointIndex;
    
    QVector<int> leftFingerJointIndices;
    QVector<int> rightFingerJointIndices;
    
    QVector<int> leftFingertipJointIndices;
    QVector<int> rightFingertipJointIndices;    
    
    glm::vec3 palmDirection;
    
    glm::vec3 neckPivot;
    
    Extents bindExtents;
    Extents staticExtents;
    Extents meshExtents;
    
    QVector<FBXAttachment> attachments;
    
    int getJointIndex(const QString& name) const { return jointIndices.value(name) - 1; }
    QStringList getJointNames() const;
};

Q_DECLARE_METATYPE(FBXGeometry)

/// Reads an FST mapping from the supplied data.
QVariantHash readMapping(const QByteArray& data);

/// Reads FBX geometry from the supplied model and mapping data.
/// \exception QString if an error occurs in parsing
FBXGeometry readFBX(const QByteArray& model, const QVariantHash& mapping);

/// Reads SVO geometry from the supplied model data.
FBXGeometry readSVO(const QByteArray& model);

#endif /* defined(__interface__FBXReader__) */
