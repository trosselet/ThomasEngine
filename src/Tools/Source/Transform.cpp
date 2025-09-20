#include "pch.h"

#include "Header/Transform.h"


void TRANSFORM::UpdateMatrix()
{
	XMStoreFloat4x4(
		&mmMatrix,
		DirectX::XMMatrixAffineTransformation(
			GetScaling(),
			XMLoadFloat3(&mvOrigin),
			GetRotation(),
			GetPosition()
		)
	);
	mIsUpdated = true;
}

TRANSFORM::TRANSFORM()
{
	Reset();
}

const DirectX::XMFLOAT3& TRANSFORM::GetPositionFLOAT() const
{
	return mvPosition;
}

DirectX::XMVECTOR TRANSFORM::GetPosition()
{
	return XMLoadFloat3(&mvPosition);
}

void TRANSFORM::SetPosition(DirectX::XMFLOAT3& _vec)
{
	mvPosition = _vec;
	mIsUpdated = false;
}

void XM_CALLCONV TRANSFORM::SetPosition(DirectX::FXMVECTOR _vec)
{
	XMStoreFloat3(
		&mvPosition,
		_vec
	);
	mIsUpdated = false;
}

void TRANSFORM::OffsetPosition(const DirectX::XMFLOAT3& _vec)
{
	DirectX::XMFLOAT3 pos = GetPositionFLOAT();
	pos.x += _vec.x;
	pos.y += _vec.y;
	pos.z += _vec.z;
	SetPosition(pos);
}

const DirectX::XMFLOAT4& TRANSFORM::GetRotationFLOAT() const
{
	return mqRotation;
}

DirectX::XMVECTOR TRANSFORM::GetRotation()
{
	return XMLoadFloat4(&mqRotation);
}

void TRANSFORM::SetRotationQuaternion(const DirectX::XMFLOAT4& _vec)
{
	mqRotation = _vec;
	DirectX::XMVECTOR qRot = XMLoadFloat4(
		&mqRotation
	);
	XMStoreFloat4x4(
		&mmRotation,
		DirectX::XMMatrixRotationQuaternion(
			qRot
		)
	);

	mvRight = { mmRotation._11, mmRotation._12, mmRotation._13 };
	mvUp = { mmRotation._21, mmRotation._22, mmRotation._23 };
	mvForward = { mmRotation._31, mmRotation._32, mmRotation._33 };
	mIsUpdated = false;
}

void XM_CALLCONV TRANSFORM::SetRotationQuaternion(DirectX::FXMVECTOR _vec)
{
	XMStoreFloat4(
		&mqRotation,
		_vec
	);
	XMStoreFloat4x4(
		&mmRotation,
		DirectX::XMMatrixRotationQuaternion(_vec)
	);
	mvRight = { mmRotation._11, mmRotation._12, mmRotation._13 };
	mvUp = { mmRotation._21, mmRotation._22, mmRotation._23 };
	mvForward = { mmRotation._31, mmRotation._32, mmRotation._33 };
	mIsUpdated = false;
}

void TRANSFORM::SetRotationYPR(const DirectX::XMFLOAT3& _ypr)
{
	DirectX::XMVECTOR RIGHT = { 1, 0, 0, 0 };
	DirectX::XMVECTOR UP = { 0, 1, 0, 0 };
	DirectX::XMVECTOR FORWARD = { 0, 0, 1, 0 };

	DirectX::XMVECTOR qRot = DirectX::XMQuaternionRotationAxis(UP, _ypr.x);
	DirectX::XMVECTOR qRotTemp;

	RIGHT = DirectX::XMVector3Rotate(RIGHT, qRot);
	qRotTemp = DirectX::XMQuaternionRotationAxis(RIGHT, _ypr.y);
	qRot = DirectX::XMQuaternionMultiply(qRot, qRotTemp);

	FORWARD = DirectX::XMVector3Rotate(FORWARD, qRot);
	qRotTemp = DirectX::XMQuaternionRotationAxis(FORWARD, _ypr.z);
	qRot = DirectX::XMQuaternionMultiply(qRot, qRotTemp);

	SetRotationQuaternion(qRot);
}

void XM_CALLCONV TRANSFORM::SetRotationYPR(DirectX::FXMVECTOR _ypr)
{
	DirectX::XMFLOAT3 YPR;
	DirectX::XMStoreFloat3(
		&YPR,
		_ypr
	);
	SetRotationYPR(YPR);
}

void TRANSFORM::RotateYPR(const DirectX::XMFLOAT3& _ypr)
{
	DirectX::XMVECTOR qDelta = DirectX::XMQuaternionRotationRollPitchYaw(_ypr.y, _ypr.x, _ypr.z);

	DirectX::XMVECTOR qCurrent = GetRotation();
	DirectX::XMVECTOR qResult = DirectX::XMQuaternionMultiply(qDelta, qCurrent);

	SetRotationQuaternion(qResult);
}

const DirectX::XMFLOAT3& TRANSFORM::GetScalingFLOAT() const
{
	return mvScaling;
}

DirectX::XMVECTOR TRANSFORM::GetScaling()
{
	return XMLoadFloat3(&mvScaling);
}

void TRANSFORM::SetScaling(const DirectX::XMFLOAT3& _vec)
{
	mvScaling = _vec;
	mIsUpdated = false;
}

void XM_CALLCONV TRANSFORM::SetScaling(DirectX::FXMVECTOR _vec)
{
	XMStoreFloat3(
		&mvScaling,
		_vec
	);
	mIsUpdated = false;
}

void TRANSFORM::Scale(const DirectX::XMFLOAT3& _scale)
{
	mvScaling.x *= _scale.x;
	mvScaling.y *= _scale.y;
	mvScaling.z *= _scale.z;
	mIsUpdated = false;
}

const DirectX::XMFLOAT4X4& TRANSFORM::GetMatrixFLOAT()
{
	if (mIsUpdated == false)
		UpdateMatrix();
	return mmMatrix;
}

DirectX::XMMATRIX TRANSFORM::GetMatrix()
{
	if (mIsUpdated == false)
		UpdateMatrix();
	return XMLoadFloat4x4(&mmMatrix);
}

DirectX::XMFLOAT3 TRANSFORM::Forward()
{
	return mvForward;
}

DirectX::XMFLOAT3 TRANSFORM::Right()
{
	return mvRight;
}

DirectX::XMFLOAT3 TRANSFORM::Up()
{
	return mvUp;
}

DirectX::XMMATRIX TRANSFORM::operator*(TRANSFORM& _other)
{
	DirectX::XMMATRIX mat = GetMatrix();
	mat *= _other.GetMatrix();
	return mat;
}

DirectX::XMMATRIX XM_CALLCONV TRANSFORM::operator*(DirectX::FXMMATRIX _other)
{
	DirectX::XMMATRIX mat = GetMatrix();
	mat *= _other;
	return mat;
}

DirectX::XMVECTOR TRANSFORM::operator*(const DirectX::XMFLOAT3& _other)
{
	return XMVector3Transform(DirectX::XMLoadFloat3(&_other), GetMatrix());
}

void TRANSFORM::Reset()
{
	mvPosition = { 0, 0, 0 };
	mvScaling = { 1, 1, 1 };

	mvRight = { 1, 0, 0 };
	mvUp = { 0, 1, 0 };
	mvForward = { 0, 0, 1 };
	mvOrigin = { 0, 0, 0 };

	XMStoreFloat4(
		&mqRotation,
		DirectX::XMQuaternionIdentity()
	);

	XMStoreFloat4x4(
		&mmRotation,
		DirectX::XMMatrixIdentity()
	);

	XMStoreFloat4x4(
		&mmMatrix,
		DirectX::XMMatrixIdentity()
	);

	mIsUpdated = true;
}