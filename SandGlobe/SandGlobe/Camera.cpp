#include "Camera.h"


Camera::Camera(void)
{
	default_location.position = XMFLOAT3(0.0,4.0,-13.0f);
	default_location.direction = XMFLOAT3(0.0,0.0,0.0);
	default_location.up = XMFLOAT3(0,1,0);
	current_camera = default_location;
	movement = default_location.position;
	moveamount = 0;
	camYaw = 0.0f;
	camPitch = 0.0f;
}

XMMATRIX Camera::getReflectMx(float reflect_y) const
{

	Camera::preset reflect = current_camera;
	reflect.position.x = movement.x;
	reflect.position.y = movement.y;
	reflect.position.z = movement.z;

	reflect.position.y = -reflect.position.y + (reflect_y*2);
	reflect.direction.y = -reflect.position.y + (reflect_y*2);
	//XMMATRIX matRotation = XMMatrixRotationRollPitchYaw( camPitch, camYaw, 0 );
	////XMVECTOR cameraRight = XMLoadFloat3(&XMFLOAT3(1,0,0));
	//XMVECTOR Camera_posV = XMLoadFloat3(&reflect.position);
	//XMVECTOR Camera_dirV = XMLoadFloat3(&reflect.direction);
	//XMVECTOR CameraUP = XMLoadFloat3(&reflect.up);
	//XMVECTOR cameraUP = XMVector3Cross(XMVector3Transform(cameraRight,matRotation),Camera_posV-Camera_dirV);
	return getCameraMx(reflect);
	//reflect.direction.z = -reflect.direction.z;
	//return getCameraMx(reflect);
}

XMFLOAT3 Camera::getposition() const
{
	return current_camera.position;
}

XMFLOAT3 Camera::getRefpos(float reflect_y) const
{
	Camera::preset reflect = current_camera;
	reflect.position.x = movement.x;
	reflect.position.y = movement.y;
	reflect.position.z = movement.z;

	reflect.position.y = -reflect.position.y + (reflect_y*2);

	return reflect.position;

}


XMMATRIX Camera::getCameraMx(const Camera::preset& camera_set) const
{
	XMVECTOR Camera_posV = XMLoadFloat3(&camera_set.position);
	XMVECTOR Camera_dirV = XMLoadFloat3(&camera_set.direction);
	XMVECTOR Camera_UPV = XMLoadFloat3(&camera_set.up);

	//XMMATRIX matRotation = XMMatrixRotationRollPitchYaw( camPitch, camYaw, 0 );
	//XMVECTOR CamX = XMVector3TransformCoord( XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f ), matRotation );
	//XMVECTOR CamY = XMVector3TransformCoord( XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ), matRotation );
	//XMVECTOR CamZ = XMVector3TransformCoord( XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ), matRotation );

	//CamZ = XMVector3Normalize( CamZ );
	//CamX = XMVector3Cross( CamY, CamZ );
	//CamX = XMVector3Normalize( CamX );
	//CamY = XMVector3Cross( CamZ, CamX );
	//CamY = XMVector3Normalize( CamY );

	//Camera_posV +=CamX * movement.x;
	//Camera_posV +=CamZ * movement.z;
	//Camera_posV +=CamY * movement.y;


	return XMMatrixLookAtLH(Camera_posV,Camera_dirV,Camera_UPV);
	/*return XMMatrixSet( 
		XMVectorGetX( CamX ), XMVectorGetX( CamY ), XMVectorGetX( CamZ ), 0.0f,
		XMVectorGetY( CamX ), XMVectorGetY( CamY ), XMVectorGetY( CamZ ), 0.0f,
		XMVectorGetZ( CamX ), XMVectorGetZ( CamY ), XMVectorGetZ( CamZ ), 0.0f,
		-XMVectorGetX( XMVector3Dot( CamX, Camera_posV ) ),
		-XMVectorGetX( XMVector3Dot( CamY, Camera_posV ) ),
		-XMVectorGetX( XMVector3Dot( CamZ, Camera_posV ) ), 1.0f );*/
}

void Camera::move(const XMFLOAT3& axis) 
{
	//moveamount+= axis.x;
	//movement.x += axis.x;
	movement.x = movement.x*cos(axis.x) + movement.z*sin(axis.x);
	movement.z = -movement.x*sin(axis.x) +movement.z*cos(axis.x);
	//XMVECTOR force = XMLoadFloat3(&axis);
	//XMVECTOR Vmovement = XMLoadFloat3(&movement);
	//XMStoreFloat3(&movement,(force));
}

void Camera::move_z(float _movement)
{
	if(movement.z <0)
	{
		movement.z += _movement;
	}
	else
	{
		movement.z -= _movement;
	}
	//current_camera.direction.y += _movement;
}

void Camera::rotate(const XMFLOAT2& input)
{
	camYaw += input.x;
	camPitch += input.y;
}

XMMATRIX Camera::getCameraMx()
{
	current_camera = current_camera;
	current_camera.position.x = movement.x;
	current_camera.position.y = movement.y;
	current_camera.position.z = movement.z;
	XMVECTOR Camera_posV = XMLoadFloat3(&current_camera.position);
	XMVECTOR Camera_dirV = XMLoadFloat3(&current_camera.direction);
	XMVECTOR Camera_UPV = XMLoadFloat3(&current_camera.up);

	//XMMATRIX matRotation = XMMatrixRotationRollPitchYaw( camPitch, camYaw, 0 );
	//XMVECTOR CamX = XMVector3TransformCoord( XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f ), matRotation );
	//XMVECTOR CamY = XMVector3TransformCoord( XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ), matRotation );
	//XMVECTOR CamZ = XMVector3TransformCoord( XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ), matRotation );

	//CamZ = XMVector3Normalize( CamZ );
	//CamX = XMVector3Cross( CamY, CamZ );
	//CamX = XMVector3Normalize( CamX );
	//CamY = XMVector3Cross( CamZ, CamX );
	//CamY = XMVector3Normalize( CamY );

	//Camera_posV =CamX * movement.x;
	//Camera_posV =CamZ * movement.z;
	//Camera_posV =CamY * movement.y;

	//movement = XMFLOAT3(0,0,0);
	return XMMatrixLookAtLH(Camera_posV,Camera_dirV,Camera_UPV);
	/*return XMMatrixSet( 
		XMVectorGetX( CamX ), XMVectorGetX( CamY ), XMVectorGetX( CamZ ), 0.0f,
		XMVectorGetY( CamX ), XMVectorGetY( CamY ), XMVectorGetY( CamZ ), 0.0f,
		XMVectorGetZ( CamX ), XMVectorGetZ( CamY ), XMVectorGetZ( CamZ ), 0.0f,
		-XMVectorGetX( XMVector3Dot( CamX, Camera_posV ) ),
		-XMVectorGetX( XMVector3Dot( CamY, Camera_posV ) ),
		-XMVectorGetX( XMVector3Dot( CamZ, Camera_posV ) ), 1.0f );*/
}

Camera::~Camera(void)
{
}

void Camera::change_camera(const XMFLOAT3& position, const XMFLOAT3& direction)
{
	current_camera.position = position;
	current_camera.direction = direction;
	movement = current_camera.position;
}

void Camera::set_sea_level(float sea_level)
{
	current_camera.position.y = sea_level;
	movement = current_camera.position;
}

void Camera::globe_view()
{
	current_camera = default_location;
	movement = current_camera.position;
}