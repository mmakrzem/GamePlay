#include "Base.h"
#include "Transform.h"
#include "Game.h"
#include "Node.h"

namespace gameplay
{

Transform::Transform()
    : _matrixDirtyBits(0), _listeners(NULL)
{
    _targetType = AnimationTarget::TRANSFORM;
    _scale.set(Vector3::one());
}

Transform::Transform(const Vector3& scale, const Quaternion& rotation, const Vector3& translation)
    : _matrixDirtyBits(0), _listeners(NULL)
{
    _targetType = AnimationTarget::TRANSFORM;
    set(scale, rotation, translation);
}

Transform::Transform(const Vector3& scale, const Matrix& rotation, const Vector3& translation)
    : _matrixDirtyBits(0), _listeners(NULL)
{
    _targetType = AnimationTarget::TRANSFORM;
    set(scale, rotation, translation);
}

Transform::Transform(const Transform& copy)
    : _matrixDirtyBits(0), _listeners(NULL)
{
    _targetType = AnimationTarget::TRANSFORM;
    set(copy);
}

Transform::~Transform()
{
    SAFE_DELETE(_listeners);
}

const Matrix& Transform::getMatrix() const
{
    if (_matrixDirtyBits)
    {
        bool hasTranslation = !_translation.isZero();
        bool hasScale = !_scale.isOne();
        bool hasRotation = !_rotation.isIdentity();

        // Compose the matrix in TRS order since we use column-major matrices with column vectors and
        // multiply M*v (as opposed to XNA and DirectX that use row-major matrices with row vectors and multiply v*M).
        if (hasTranslation || (_matrixDirtyBits & DIRTY_TRANSLATION) == DIRTY_TRANSLATION)
        {
            Matrix::createTranslation(_translation, &_matrix);
            if (hasRotation || (_matrixDirtyBits & DIRTY_ROTATION) == DIRTY_ROTATION)
            {
                _matrix.rotate(_rotation);
            }
            if (hasScale || (_matrixDirtyBits & DIRTY_SCALE) == DIRTY_SCALE)
            {
                _matrix.scale(_scale);
            }
        }
        else if (hasRotation || (_matrixDirtyBits & DIRTY_ROTATION) == DIRTY_ROTATION)
        {
            Matrix::createRotation(_rotation, &_matrix);
            if (hasScale || (_matrixDirtyBits & DIRTY_SCALE) == DIRTY_SCALE)
            {
                _matrix.scale(_scale);
            }
        }
        else if (hasScale || (_matrixDirtyBits & DIRTY_SCALE) == DIRTY_SCALE)
        {
            Matrix::createScale(_scale, &_matrix);
        }

        _matrixDirtyBits = 0;
    }

    return _matrix;
}

const Vector3& Transform::getScale() const
{
    return _scale;
}

void Transform::getScale(Vector3* scale) const
{
    scale->set(_scale);
}

float Transform::getScaleX() const
{
    return _scale.x;
}

float Transform::getScaleY() const
{
    return _scale.y;
}

float Transform::getScaleZ() const
{
    return _scale.z;
}

const Quaternion& Transform::getRotation() const
{
    return _rotation;
}

void Transform::getRotation(Quaternion* rotation) const
{
    assert(rotation);

    rotation->set(_rotation);
}

void Transform::getRotation(Matrix* rotation) const
{
    assert(rotation);

    Matrix::createRotation(_rotation, rotation);
}

float Transform::getRotation(Vector3* axis) const
{
    assert(axis);
    return _rotation.toAxisAngle(axis);
}

const Vector3& Transform::getTranslation() const
{
    return _translation;
}

void Transform::getTranslation(Vector3* translation) const
{
    translation->set(_translation);
}

float Transform::getTranslationX() const
{
    return _translation.x;
}

float Transform::getTranslationY() const
{
    return _translation.y;
}

float Transform::getTranslationZ() const
{
    return _translation.z;
}

Vector3 Transform::getForwardVector() const
{
    Vector3 v;
    getForwardVector(&v);
    return v;
}

void Transform::getForwardVector(Vector3* dst) const
{
    getMatrix().getForwardVector(dst);
}

Vector3 Transform::getBackVector() const
{
    Vector3 v;
    getBackVector(&v);
    return v;
}

void Transform::getBackVector(Vector3* dst) const
{
    getMatrix().getBackVector(dst);
}

Vector3 Transform::getUpVector() const
{
    Vector3 v;
    getUpVector(&v);
    return v;
}

void Transform::getUpVector(Vector3* dst) const
{
    getMatrix().getUpVector(dst);
}

Vector3 Transform::getDownVector() const
{
    Vector3 v;
    getDownVector(&v);
    return v;
}

void Transform::getDownVector(Vector3* dst) const
{
    getMatrix().getDownVector(dst);
}

Vector3 Transform::getLeftVector() const
{
    Vector3 v;
    getLeftVector(&v);
    return v;
}

void Transform::getLeftVector(Vector3* dst) const
{
    getMatrix().getLeftVector(dst);
}

Vector3 Transform::getRightVector() const
{
    Vector3 v;
    getRightVector(&v);
    return v;
}

void Transform::getRightVector(Vector3* dst) const
{
    getMatrix().getRightVector(dst);
}

void Transform::rotate(float qx, float qy, float qz, float qw)
{
    Quaternion q(qx, qy, qz, qw);
    _rotation.multiply(q);
    dirty(DIRTY_ROTATION);
}

void Transform::rotate(const Quaternion& rotation)
{
    _rotation.multiply(rotation);
    dirty(DIRTY_ROTATION);
}

void Transform::rotate(const Vector3& axis, float angle)
{
    Quaternion rotationQuat;
    Quaternion::createFromAxisAngle(axis, angle, &rotationQuat);
    _rotation.multiply(rotationQuat);
    _rotation.normalize();
    dirty(DIRTY_ROTATION);
}

void Transform::rotate(const Matrix& rotation)
{
    Quaternion rotationQuat;
    Quaternion::createFromRotationMatrix(rotation, &rotationQuat);
    _rotation.multiply(rotationQuat);
    dirty(DIRTY_ROTATION);
}

void Transform::rotateX(float angle)
{
    Quaternion rotationQuat;
    Quaternion::createFromAxisAngle(Vector3::unitX(), angle, &rotationQuat);
    _rotation.multiply(rotationQuat);
    dirty(DIRTY_ROTATION);
}

void Transform::rotateY(float angle)
{
    Quaternion rotationQuat;
    Quaternion::createFromAxisAngle(Vector3::unitY(), angle, &rotationQuat);
    _rotation.multiply(rotationQuat);
    dirty(DIRTY_ROTATION);
}

void Transform::rotateZ(float angle)
{
    Quaternion rotationQuat;
    Quaternion::createFromAxisAngle(Vector3::unitZ(), angle, &rotationQuat);
    _rotation.multiply(rotationQuat);
    dirty(DIRTY_ROTATION);
}

void Transform::scale(float scale)
{
    _scale.scale(scale);
    dirty(DIRTY_SCALE);
}

void Transform::scale(float sx, float sy, float sz)
{
    _scale.x *= sx;
    _scale.y *= sy;
    _scale.z *= sz;
    dirty(DIRTY_SCALE);
}

void Transform::scale(const Vector3& scale)
{
    _scale.x *= scale.x;
    _scale.y *= scale.y;
    _scale.z *= scale.z;
    dirty(DIRTY_SCALE);
}

void Transform::scaleX(float sx)
{
    _scale.x *= sx;
    dirty(DIRTY_SCALE);
}

void Transform::scaleY(float sy)
{
    _scale.y *= sy;
    dirty(DIRTY_SCALE);
}

void Transform::scaleZ(float sz)
{
    _scale.z *= sz;
    dirty(DIRTY_SCALE);
}

void Transform::set(const Vector3& scale, const Quaternion& rotation, const Vector3& translation)
{
    _scale.set(scale);
    _rotation.set(rotation);
    _translation.set(translation);
    dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
}

void Transform::set(const Vector3& scale, const Matrix& rotation, const Vector3& translation)
{
    _scale.set(scale);
    Quaternion rotationQuat;
    Quaternion::createFromRotationMatrix(rotation, &rotationQuat);
    _rotation.set(rotationQuat);
    _translation.set(translation);
    dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
}

void Transform::set(const Vector3& scale, const Vector3& axis, float angle, const Vector3& translation)
{
    _scale.set(scale);
    _rotation.set(axis, angle);
    _translation.set(translation);
    dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
}

void Transform::set(const Transform& transform)
{
    _scale.set(transform._scale);
    _rotation.set(transform._rotation);
    _translation.set(transform._translation);
    dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
}

void Transform::setIdentity()
{
    _scale.set(1.0f, 1.0f, 1.0f);
    _rotation.setIdentity();
    _translation.set(0.0f, 0.0f, 0.0f);
    dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
}

void Transform::setScale(float scale)
{
    _scale.set(scale, scale, scale);
    dirty(DIRTY_SCALE);
}

void Transform::setScale(float sx, float sy, float sz)
{
    _scale.set(sx, sy, sz);
    dirty(DIRTY_SCALE);
}

void Transform::setScale(const Vector3& scale)
{
    _scale.set(scale);
    dirty(DIRTY_SCALE);
}

void Transform::setScaleX(float sx)
{
    _scale.x = sx;
    dirty(DIRTY_SCALE);
}

void Transform::setScaleY(float sy)
{
    _scale.y = sy;
    dirty(DIRTY_SCALE);
}

void Transform::setScaleZ(float sz)
{
    _scale.z = sz;
    dirty(DIRTY_SCALE);
}

void Transform::setRotation(const Quaternion& rotation)
{
    _rotation.set(rotation);
    dirty(DIRTY_ROTATION);
}

void Transform::setRotation(float qx, float qy, float qz, float qw)
{
    _rotation.set(qx, qy, qz, qw);
    dirty(DIRTY_ROTATION);
}

void Transform::setRotation(const Matrix& rotation)
{
    Quaternion rotationQuat;
    Quaternion::createFromRotationMatrix(rotation, &rotationQuat);
    _rotation.set(rotationQuat);
    dirty(DIRTY_ROTATION);
}

void Transform::setRotation(const Vector3& axis, float angle)
{
    _rotation.set(axis, angle);
    dirty(DIRTY_ROTATION);
}

void Transform::setTranslation(const Vector3& translation)
{
    _translation.set(translation);
    dirty(DIRTY_TRANSLATION);
}

void Transform::setTranslation(float tx, float ty, float tz)
{
    _translation.set(tx, ty, tz);
    dirty(DIRTY_TRANSLATION);
}

void Transform::setTranslationX(float tx)
{
    _translation.x = tx;
    dirty(DIRTY_TRANSLATION);
}

void Transform::setTranslationY(float ty)
{
    _translation.y = ty;
    dirty(DIRTY_TRANSLATION);
}

void Transform::setTranslationZ(float tz)
{
    _translation.z = tz;
    dirty(DIRTY_TRANSLATION);
}

void Transform::translate(float tx, float ty, float tz)
{
    _translation.x += tx;
    _translation.y += ty;
    _translation.z += tz;
    dirty(DIRTY_TRANSLATION);
}

void Transform::translate(const Vector3& translation)
{
    _translation.x += translation.x;
    _translation.y += translation.y;
    _translation.z += translation.z;
    dirty(DIRTY_TRANSLATION);
}

void Transform::translateX(float tx)
{
    _translation.x += tx;
    dirty(DIRTY_TRANSLATION);
}

void Transform::translateY(float ty)
{
    _translation.y += ty;
    dirty(DIRTY_TRANSLATION);
}

void Transform::translateZ(float tz)
{
    _translation.z += tz;
    dirty(DIRTY_TRANSLATION);
}

void Transform::translateLeft(float amount)
{
    // Force the current transform matrix to be updated.
    getMatrix();

    Vector3 left;
    _matrix.getLeftVector(&left);
    left.normalize();
    left.scale(amount);

    translate(left);
}

void Transform::translateUp(float amount)
{
    // Force the current transform matrix to be updated.
    getMatrix();

    Vector3 up;
    _matrix.getUpVector(&up);
    up.normalize();
    up.scale(amount);

    translate(up);
}

void Transform::translateForward(float amount)
{
    // Force the current transform matrix to be updated.
    getMatrix();

    Vector3 forward;
    _matrix.getForwardVector(&forward);
    forward.normalize();
    forward.scale(amount);

    translate(forward);
}

void Transform::transformPoint(Vector3* point)
{
    assert(point);

    getMatrix();
    _matrix.transformPoint(point);
}

void Transform::transformPoint(const Vector3& point, Vector3* dst)
{
    assert(dst);

    getMatrix();
    _matrix.transformPoint(point, dst);
}

void Transform::transformVector(Vector3* normal)
{
    assert(normal);

    getMatrix();
    _matrix.transformVector(normal);
}

void Transform::transformVector(const Vector3& normal, Vector3* dst)
{
    getMatrix();
    _matrix.transformVector(normal, dst);
}

void Transform::transformVector(float x, float y, float z, float w, Vector3* dst)
{
    getMatrix();
    _matrix.transformVector(x, y, z, w, dst);
}

unsigned int Transform::getAnimationPropertyComponentCount(int propertyId) const
{
    switch (propertyId)
    {
        case ANIMATE_SCALE_UNIT:
        case ANIMATE_SCALE_X:
        case ANIMATE_SCALE_Y:
        case ANIMATE_SCALE_Z:
        case ANIMATE_TRANSLATE_X:
        case ANIMATE_TRANSLATE_Y:
        case ANIMATE_TRANSLATE_Z:
            return 1;
        case ANIMATE_SCALE:
        case ANIMATE_TRANSLATE:
            return 3;
        case ANIMATE_ROTATE:
            return 4;
        case ANIMATE_ROTATE_TRANSLATE:
            return 7;
        case ANIMATE_SCALE_ROTATE_TRANSLATE:
            return 10;
        default:
            return -1;
    }
}

void Transform::getAnimationPropertyValue(int propertyId, AnimationValue* value)
{
    switch (propertyId)
    {
        case ANIMATE_SCALE_UNIT:
            value->setFloat(0, _scale.x);
            break;
        case ANIMATE_SCALE:
            value->setFloat(0, _scale.x);
            value->setFloat(1, _scale.y);
            value->setFloat(2, _scale.z);
            break;
        case ANIMATE_SCALE_X:
            value->setFloat(0, _scale.x);
            break;
        case ANIMATE_SCALE_Y:
            value->setFloat(0, _scale.y);
            break;
        case ANIMATE_SCALE_Z:
            value->setFloat(0, _scale.z);
            break;
        case ANIMATE_ROTATE:
            value->setFloat(0, _rotation.x);
            value->setFloat(1, _rotation.y);
            value->setFloat(2, _rotation.z);
            value->setFloat(3, _rotation.w);
            break;
        case ANIMATE_TRANSLATE:
            value->setFloat(0, _translation.x);
            value->setFloat(1, _translation.y);
            value->setFloat(2, _translation.z);
            break;
        case ANIMATE_TRANSLATE_X:
            value->setFloat(0, _translation.x);
            break;
        case ANIMATE_TRANSLATE_Y:
            value->setFloat(0, _translation.y);
            break;
        case ANIMATE_TRANSLATE_Z:
            value->setFloat(0, _translation.z);
            break;
        case ANIMATE_ROTATE_TRANSLATE:
            value->setFloat(0, _rotation.x);
            value->setFloat(1, _rotation.y);
            value->setFloat(2, _rotation.z);
            value->setFloat(3, _rotation.w);
            value->setFloat(4, _translation.x);
            value->setFloat(5, _translation.y);
            value->setFloat(6, _translation.z);
            break;
        case ANIMATE_SCALE_ROTATE_TRANSLATE:
            value->setFloat(0, _scale.x);
            value->setFloat(1, _scale.y);
            value->setFloat(2, _scale.z);
            value->setFloat(3, _rotation.x);
            value->setFloat(4, _rotation.y);
            value->setFloat(5, _rotation.z);
            value->setFloat(6, _rotation.w);
            value->setFloat(7, _translation.x);
            value->setFloat(8, _translation.y);
            value->setFloat(9, _translation.z);
            break;
        default:
            break;
    }
}

void Transform::setAnimationPropertyValue(int propertyId, AnimationValue* value, float blendWeight)
{
    assert(blendWeight >= 0.0f && blendWeight <= 1.0f);

    switch (propertyId)
    {
        case ANIMATE_SCALE_UNIT:
        {
            applyAnimationValueScaleX(value->getFloat(0), blendWeight);
            applyAnimationValueScaleY(value->getFloat(0), blendWeight);
            applyAnimationValueScaleZ(value->getFloat(0), blendWeight);
            break;
        }   
        case ANIMATE_SCALE:
        {
            applyAnimationValueScaleX(value->getFloat(0), blendWeight);
            applyAnimationValueScaleY(value->getFloat(1), blendWeight);
            applyAnimationValueScaleZ(value->getFloat(2), blendWeight);
            break;
        }
        case ANIMATE_SCALE_X:
        {
            applyAnimationValueScaleX(value->getFloat(0), blendWeight);
            break;
        }
        case ANIMATE_SCALE_Y:
        {
            applyAnimationValueScaleY(value->getFloat(0), blendWeight);
            break;
        }
        case ANIMATE_SCALE_Z:
        {
            applyAnimationValueScaleZ(value->getFloat(0), blendWeight);
            break;
        }
        case ANIMATE_ROTATE:
        {
            Quaternion q(value->getFloat(0), value->getFloat(1), value->getFloat(2), value->getFloat(3));
            applyAnimationValueRotation(&q, blendWeight);
            break;
        }
        case ANIMATE_TRANSLATE:
        {
            applyAnimationValueTranslationX(value->getFloat(0), blendWeight);
            applyAnimationValueTranslationY(value->getFloat(1), blendWeight);
            applyAnimationValueTranslationZ(value->getFloat(2), blendWeight);
            break;
        }
        case ANIMATE_TRANSLATE_X:
        {
            applyAnimationValueTranslationX(value->getFloat(0), blendWeight);
            break;
        }
        case ANIMATE_TRANSLATE_Y:
        {
            applyAnimationValueTranslationY(value->getFloat(0), blendWeight);
            break;
        }
        case ANIMATE_TRANSLATE_Z:
        {
            applyAnimationValueTranslationZ(value->getFloat(0), blendWeight);
            break;
        }
        case ANIMATE_ROTATE_TRANSLATE:
        {
            Quaternion q(value->getFloat(0), value->getFloat(1), value->getFloat(2), value->getFloat(3));
            applyAnimationValueRotation(&q, blendWeight);
            applyAnimationValueTranslationX(value->getFloat(4), blendWeight);
            applyAnimationValueTranslationY(value->getFloat(5), blendWeight);
            applyAnimationValueTranslationZ(value->getFloat(6), blendWeight);
            break;
        }
        case ANIMATE_SCALE_ROTATE_TRANSLATE:
        {
            applyAnimationValueScaleX(value->getFloat(0), blendWeight);
            applyAnimationValueScaleY(value->getFloat(1), blendWeight);
            applyAnimationValueScaleZ(value->getFloat(2), blendWeight);
            Quaternion q(value->getFloat(3), value->getFloat(4), value->getFloat(5), value->getFloat(6));
            applyAnimationValueRotation(&q, blendWeight);
            applyAnimationValueTranslationX(value->getFloat(7), blendWeight);
            applyAnimationValueTranslationY(value->getFloat(8), blendWeight);
            applyAnimationValueTranslationZ(value->getFloat(9), blendWeight);
            break;
        }
        default:
            break;
    }
}

void Transform::dirty(char matrixDirtyBits)
{
    _matrixDirtyBits |= matrixDirtyBits;
    transformChanged();
}

void Transform::addListener(Transform::Listener* listener, long cookie)
{
    if (_listeners == NULL)
        _listeners = new std::list<TransformListener>();

    TransformListener l;
    l.listener = listener;
    l.cookie = cookie;
    _listeners->push_back(l);
}

void Transform::removeListener(Transform::Listener* listener)
{
    if (_listeners)
    {
        for (std::list<TransformListener>::iterator itr = _listeners->begin(); itr != _listeners->end(); itr++)
        {
            if ((*itr).listener == listener)
            {
                _listeners->erase(itr);
                break;
            }
        }
    }
}

void Transform::transformChanged()
{
    if (_listeners)
    {
        for (std::list<TransformListener>::iterator itr = _listeners->begin(); itr != _listeners->end(); itr++)
        {
            TransformListener& l = *itr;
            l.listener->transformChanged(this, l.cookie);
        }
    }
}

void Transform::cloneInto(Transform* transform, NodeCloneContext &context) const
{
    AnimationTarget::cloneInto(transform, context);
    transform->_scale.set(_scale);
    transform->_rotation.set(_rotation);
    transform->_translation.set(_translation);
}

void Transform::applyAnimationValueScaleX(float sx, float blendWeight)
{
    if ((_animationPropertyBitFlag & ANIMATION_SCALE_X_BIT) != ANIMATION_SCALE_X_BIT)
        _animationPropertyBitFlag |= ANIMATION_SCALE_X_BIT;
    else
        sx = Curve::lerp(blendWeight, _scale.x, sx);

    setScaleX(sx);
}

void Transform::applyAnimationValueScaleY(float sy, float blendWeight)
{
    if ((_animationPropertyBitFlag & ANIMATION_SCALE_Y_BIT) != ANIMATION_SCALE_Y_BIT)
        _animationPropertyBitFlag |= ANIMATION_SCALE_Y_BIT;
    else
        sy = Curve::lerp(blendWeight, _scale.y, sy);

    setScaleY(sy);
}

void Transform::applyAnimationValueScaleZ(float sz, float blendWeight)
{
    if ((_animationPropertyBitFlag & ANIMATION_SCALE_Z_BIT) != ANIMATION_SCALE_Z_BIT)
        _animationPropertyBitFlag |= ANIMATION_SCALE_Z_BIT;
    else
        sz = Curve::lerp(blendWeight, _scale.z, sz);

    setScaleZ(sz);
}

void Transform::applyAnimationValueRotation(Quaternion* q, float blendWeight)
{
    if ((_animationPropertyBitFlag & ANIMATION_ROTATION_BIT) != ANIMATION_ROTATION_BIT)
        _animationPropertyBitFlag |= ANIMATION_ROTATION_BIT;
    else
        Quaternion::slerp(_rotation, *q, blendWeight, q);
     
    setRotation(*q);
}

void Transform::applyAnimationValueTranslationX(float tx, float blendWeight)
{
    if ((_animationPropertyBitFlag & ANIMATION_TRANSLATION_X_BIT) != ANIMATION_TRANSLATION_X_BIT)
        _animationPropertyBitFlag |= ANIMATION_TRANSLATION_X_BIT;
    else
        tx = Curve::lerp(blendWeight, _translation.x, tx);

    setTranslationX(tx);
}

void Transform::applyAnimationValueTranslationY(float ty, float blendWeight)
{
    if ((_animationPropertyBitFlag & ANIMATION_TRANSLATION_Y_BIT) != ANIMATION_TRANSLATION_Y_BIT)
        _animationPropertyBitFlag |= ANIMATION_TRANSLATION_Y_BIT;
    else
        ty = Curve::lerp(blendWeight, _translation.y, ty);

    setTranslationY(ty);
}

void Transform::applyAnimationValueTranslationZ(float tz, float blendWeight)
{
    if ((_animationPropertyBitFlag & ANIMATION_TRANSLATION_Z_BIT) != ANIMATION_TRANSLATION_Z_BIT)
        _animationPropertyBitFlag |= ANIMATION_TRANSLATION_Z_BIT;
    else
        tz = Curve::lerp(blendWeight, _translation.z, tz);

    setTranslationZ(tz);
}

}
