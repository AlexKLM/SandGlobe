#pragma once
#include <windows.h>
#include <xnamath.h>
class Camera
{
private:
	struct preset{
		XMFLOAT3 position;
		XMFLOAT3 direction;
		XMFLOAT3 up;
	};
	preset current_camera;
	preset default_location;
	XMFLOAT3 movement;
	float moveamount, camYaw,camPitch;
	
public:
	XMMATRIX getReflectMx(float reflect_y)const;
	XMFLOAT3 getRefpos(float reflect_y) const;
	XMFLOAT3 getposition() const;
	XMMATRIX getCameraMx();
	XMMATRIX getCameraMx(const preset& camera_set) const;
	XMMATRIX getInvertedMx();
	void move_z(float _movement);
	void move(const XMFLOAT3& axis);
	void rotate(const XMFLOAT2& input);
	void change_camera(const XMFLOAT3& position,const XMFLOAT3& direction);
	void set_sea_level(float sea_level);//mainly for sea level camera
	void globe_view(); //go to default location
	Camera(void);
	~Camera(void);
};

