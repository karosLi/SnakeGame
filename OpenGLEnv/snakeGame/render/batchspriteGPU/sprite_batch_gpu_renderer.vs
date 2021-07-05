#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
/**
 实例化数组，每绘制一次实例，就更新一次属性值
 */
layout (location = 2) in vec2 aInstancePosition;
layout (location = 3) in vec2 aInstanceSize;
layout (location = 4) in float aInstanceRadian;
layout (location = 5) in vec4 aInstanceQuaternion;
layout (location = 6) in int aInstanceTexIndex;

out vec2 TexCoords;
flat out int TexIndex;

uniform mat4 projection;

/**
 参考
 
 https://en.wikibooks.org/wiki/GLSL_Programming/Vector_and_Matrix_Operations
 https://stackoverflow.com/questions/46776217/compute-translation-mattrix-from-position-directly-in-shader-with-glsl
 https://stackoverflow.com/questions/24741050/recommended-way-to-approach-rotations-in-opengl
 https://www.geeks3d.com/20141201/how-to-rotate-a-vertex-by-a-quaternion-in-glsl/
 */

// 四元数旋转变换
vec3 quat_transform(vec4 quat, vec3 pos)
{
    return pos + 2.0 * cross(quat.xyz, cross(quat.xyz, pos) + quat.w * pos);
}

// 四元数转矩阵
mat3 mat3_cast(vec4 quat)
{
    mat3 result = mat3(1);
    float qxx = quat.x * quat.x;
    float qyy = quat.y * quat.y;
    float qzz = quat.z * quat.z;
    float qxz = quat.x * quat.z;
    float qxy = quat.x * quat.y;
    float qyz = quat.y * quat.z;
    float qwx = quat.w * quat.x;
    float qwy = quat.w * quat.y;
    float qwz = quat.w * quat.z;
    
    result[0][0] = 1 - 2 * (qyy + qzz);
    result[0][1] = 2 * (qxy + qwz);
    result[0][2] = 2 * (qxz - qwy);

    result[1][0] = 2 * (qxy - qwz);
    result[1][1] = 1 - 2 * (qxx +  qzz);
    result[1][2] = 2 * (qyz + qwx);

    result[2][0] = 2 * (qxz + qwy);
    result[2][1] = 2 * (qyz - qwx);
    result[2][2] = 1 - 2 * (qxx +  qyy);
    
    return result;
}

// 旋转变换
vec3 rotation_transform(vec4 quat, float radians, vec3 pos)
{
    if (radians > 0) {
        mat3 rotationMatrix = mat3(
            vec3(cos(radians),
                 sin(radians),
                 0.0),
            vec3(-sin(radians),
                 cos(radians),
                 0.0),
            vec3(0.0,
                 0.0,
                 1.0));
        
        return rotationMatrix * pos;
    }
    
    return quat_transform(quat, pos);
}

// 旋转矩阵
mat4 rotation_matrix(vec4 quat, float radians)
{
    if (radians > 0) {
        mat3 rotationMatrix = mat3(
            vec3(cos(radians),
                 sin(radians),
                 0.0),
            vec3(-sin(radians),
                 cos(radians),
                 0.0),
            vec3(0.0,
                 0.0,
                 1.0));
        
        return mat4(rotationMatrix);
    }
    
    return mat4(mat3_cast(quat));
}

void main()
{
    /**
     c0  c1  c2  c3
   [ Xx  Yx  Zx  Tx ]
   [ Xy  Yy  Zy  Ty ]
   [ Xz  Yz  Zz  Tz ]
   [  0   0   0   1 ]
     */
    mat4 modelMatrix = mat4(1.0);
    
    // 缩放
    mat4 scaleMatrix = mat4(
         vec4(aInstanceSize.x,
              0.0,
              0.0,
              0.0),
         vec4(0.0,
              aInstanceSize.y,
              0.0,
              0.0),
         vec4(0.0,
              0.0,
              1.0,
              0.0),
         vec4(0.0,
              0.0,
              0.0,
              1.0));
    
    // 平移到原点
    mat4 translateToOriginMatrix = mat4(
         vec4(1.0,
              0.0,
              0.0,
              0.0),
         vec4(0.0,
              1.0,
              0.0,
              0.0),
         vec4(0.0,
              0.0,
              1.0,
              0.0),
         vec4(-0.5 * aInstanceSize.x,
              -0.5 * aInstanceSize.y,
              0.0,
              1.0));
    
    // 在原点旋转
    mat4 rotationMatrix = rotation_matrix(aInstanceQuaternion, aInstanceRadian);
    
    // 平移回初始位置
    mat4 translateToStartMatrix = mat4(
         vec4(1.0,
              0.0,
              0.0,
              0.0),
         vec4(0.0,
              1.0,
              0.0,
              0.0),
         vec4(0.0,
              0.0,
              1.0,
              0.0),
         vec4(0.5 * aInstanceSize.x,
              0.5 * aInstanceSize.y,
              0.0,
              1.0));
    
    // 平移到目标位置
    mat4 translateToTargetMatrix = mat4(
         vec4(1.0,
              0.0,
              0.0,
              0.0),
         vec4(0.0,
              1.0,
              0.0,
              0.0),
         vec4(0.0,
              0.0,
              1.0,
              0.0),
         vec4(aInstancePosition.x,
              aInstancePosition.y,
              0.0,
              1.0));
    
    modelMatrix = translateToTargetMatrix * translateToStartMatrix * rotationMatrix * translateToOriginMatrix * scaleMatrix;
    
    TexCoords = aTexCoord;
    TexIndex = aInstanceTexIndex;
    gl_Position = projection * modelMatrix * vec4(aPos, 1.0);
}
