#define M_PI 3.1415926535
#define RadianToURotation 180.0f / M_PI

struct FText
{
	char _padding_[ 0x28 ];
	PWCHAR Name;
	DWORD Length;
};

struct State
{
	uint64_t Keys[ 7 ];
};

struct FQuat {
	float x;
	float y;
	float z;
	float w;
};

struct FMinimalViewInfo
{
	Vector3 Location; //+ 0x1260
	Vector3 Rotation; //+ 0x126C
	float FOV;     //+ 0x1278
};
enum class EAresOutlineMode : uint8_t
{
	None = 0,
	Outline = 1,
	Block = 2,
	Enemy = 3,
	AlwaysOutline = 4,
	AlwaysEnemy = 5,
	EAresOutlineMode_MAX = 6
};
enum class DamageSectionType : uint8_t {
	Health = 0,
	Shield = 1,
	Temporary = 2,
	Count = 3,
	DamageSectionType_MAX = 4
};

struct box_bounds {
	float left, right, top, bottom;

	bool empty( ) {
		if ( this->left == 0 && this->right == 0 && this->top == 0 && this->bottom == 0 )
			return true;

		if ( this->left == FLT_MAX || this->right == FLT_MIN || this->top == FLT_MAX || this->bottom == FLT_MIN )
			return true;

		return false;
	}

	static box_bounds null( ) {
		return { 0, 0, 0, 0 };
	}

	static box_bounds max_( )
	{
		return { FLT_MAX, FLT_MIN, FLT_MAX, FLT_MIN };
	}
};

struct FTransform {
	FQuat rot;
	Vector3 translation;
	char pad[ 4 ];
	Vector3 scale;
	char pad1[ 4 ];
	D3DMATRIX ToMatrixWithScale( ) {
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = ( 1.0f - ( yy2 + zz2 ) ) * scale.x;
		m._22 = ( 1.0f - ( xx2 + zz2 ) ) * scale.y;
		m._33 = ( 1.0f - ( xx2 + yy2 ) ) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = ( yz2 - wx2 ) * scale.z;
		m._23 = ( yz2 + wx2 ) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = ( xy2 - wz2 ) * scale.y;
		m._12 = ( xy2 + wz2 ) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = ( xz2 + wy2 ) * scale.z;
		m._13 = ( xz2 - wy2 ) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

D3DMATRIX MatrixMultiplication( D3DMATRIX pM1, D3DMATRIX pM2 )
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}

class APlayerCameraManager
{
private:
	static D3DMATRIX toMatrix( Vector3 rot, Vector3 origin = Vector3::Zero( ) )
	{
		float Pitch = ( rot.x * float( M_PI ) / 180.f );
		float Yaw = ( rot.y * float( M_PI ) / 180.f );
		float Roll = ( rot.z * float( M_PI ) / 180.f );

		float SP = sinf( Pitch );
		float CP = cosf( Pitch );
		float SY = sinf( Yaw );
		float CY = cosf( Yaw );
		float SR = sinf( Roll );
		float CR = cosf( Roll );

		D3DMATRIX Matrix;
		Matrix._11 = CP * CY;
		Matrix._12 = CP * SY;
		Matrix._13 = SP;
		Matrix._14 = 0.f;

		Matrix._21 = SR * SP * CY - CR * SY;
		Matrix._22 = SR * SP * SY + CR * CY;
		Matrix._23 = -SR * CP;
		Matrix._24 = 0.f;

		Matrix._31 = -( CR * SP * CY + SR * SY );
		Matrix._32 = CY * SR - CR * SP * SY;
		Matrix._33 = CR * CP;
		Matrix._34 = 0.f;

		Matrix._41 = origin.x;
		Matrix._42 = origin.y;
		Matrix._43 = origin.z;
		Matrix._44 = 1.f;

		return Matrix;
	}
public:
	Vector3 position( ) 
	{
		if ( !this ) return Vector3::Zero( );
		return kernel->read<Vector3>( this + offsets::camera_position );
	}

	Vector3 rotation( ) 
	{
		if ( !this ) return Vector3::Zero( );
		return kernel->read<Vector3>( this + offsets::camera_rotation );
	}

	float fov( )
	{
		if ( !this ) return 0;
		return kernel->read<float>( this + offsets::camera_fov );
	}

	float zoom()
	{
		float fov = this->fov();

		if (fov == 103.0f) return 1.0f;
		else if (fov == 89.565216f) return 1.15f;
		else if (fov == 82.400002f) return 1.25f;
		else if (fov == 68.666664f) return 1.5f;
		else if (fov == 41.200001f) return 2.5f;
		else if (fov == 29.428572f) return 3.5f;
		else if (fov == 20.600000f) return 5.0f;
		else return 1.0f;
	}

	bool world_to_screen( Vector3 world_pos, Vector2& screen_pos )
	{
		if ( !this ) return false;
		D3DMATRIX tempMatrix = toMatrix( this->rotation( ) );

		Vector3 vAxisX, vAxisY, vAxisZ;

		vAxisX = Vector3( tempMatrix.m[ 0 ][ 0 ], tempMatrix.m[ 0 ][ 1 ], tempMatrix.m[ 0 ][ 2 ] );
		vAxisY = Vector3( tempMatrix.m[ 1 ][ 0 ], tempMatrix.m[ 1 ][ 1 ], tempMatrix.m[ 1 ][ 2 ] );
		vAxisZ = Vector3( tempMatrix.m[ 2 ][ 0 ], tempMatrix.m[ 2 ][ 1 ], tempMatrix.m[ 2 ][ 2 ] );

		Vector3 vDelta = world_pos - this->position( );
		Vector3 vTransformed = Vector3( vDelta.dot_product( vAxisY ), vDelta.dot_product( vAxisZ ), vDelta.dot_product( vAxisX ) );

		if ( vTransformed.z < .1f )
			vTransformed.z = .1f;

		float FovAngle = this->fov( );

		screen_pos.x = renderer->screen_center.x + vTransformed.x * ( renderer->screen_center.x / tanf( FovAngle * (float) M_PI / 360.f ) ) / vTransformed.z;
		screen_pos.y = renderer->screen_center.y - vTransformed.y * ( renderer->screen_center.x / tanf( FovAngle * (float) M_PI / 360.f ) ) / vTransformed.z;
			
		return true;
	}
};

static APlayerCameraManager* camera_manager = nullptr;

template<class T>
class TArray
{
public:

	T get( uint32_t idx )
	{
		return kernel->read< T >( this + ( idx * sizeof( T ) ) );
	}

	uint32_t size( )
	{
		return kernel->read< uint32_t >( this + sizeof( T ) + 0x8 );
	}
};

class UDamageSectionComponent
{
public:
	DamageSectionType type( )
	{
		if ( !this ) return DamageSectionType::Count;
		return kernel->read<DamageSectionType>( this + offsets::damage_section_type );
	}

	float life( )
	{
		if ( !this ) return 0;
		return kernel->read<float>( this + offsets::damage_section_life );
	}

	float max_life( )
	{
		if ( !this ) return 0;
		return kernel->read<float>( this + offsets::damage_section_max_life );
	}
};

template<class T>
class TArrayT
{
public:
	T get( uint32_t idx )
	{
		return kernel->read< T >( this->Data + idx * sizeof( T ) );
	}

	uint32_t size( )
	{
		return this->Count;
	}

	template<typename F>
	void for_each( const F callback )
	{
		for ( auto i = 0; i < this->Count; i++ )
		{
			auto object = this->get( i );
			if ( !object )
				continue;
			callback( object, i );
		}
	}

protected:
	T Data;
	int Count;
	int Max;
};


class UDamageableComponent
{
public:
	TArrayT<UDamageSectionComponent*> damage_sections( )
	{
		return kernel->read<TArrayT<UDamageSectionComponent*>>( this + offsets::damage_sections );
	}

	std::tuple<float, float> shield( )
	{
		float shield, max_shield = 0.0f;

		if ( !this ) return std::make_tuple(shield, max_shield );

		auto sections = this->damage_sections( );
		if ( sections.size() <= 0 ) return std::make_tuple( shield, max_shield );
		sections.for_each( [ & ]( UDamageSectionComponent* section, uint32_t idx ) {
			if ( section->type( ) != DamageSectionType::Shield ) return;
			//if ( section->max_life( ) <= 0.0f || section->max_life( ) > 1000.0f || section->life( ) < 0 || section->life( ) > 1000.0f ) return;
			//if ( max_shield > 0 && max_shield <= 50 ) return;

			shield = section->life( );
			max_shield = section->max_life( );
		});

		return std::make_tuple(shield, max_shield);
	}

	float health( )
	{
		if ( !this ) return 0;
		return kernel->read<float>( this + offsets::current_health );
	}
};

class TeamComponent //UBaseTeamComponent 
{
public:
	int team_id( )
	{
		if ( !this ) return 0;
		return kernel->read<int>( this + offsets::team_id );
	}
};

class PlayerState
{
public:
	TeamComponent* team_component( )
	{
		if ( !this ) return nullptr;
		return kernel->read<TeamComponent*>( this + offsets::team_component );
	}
};

class USceneComponent
{
public:
	//EAresOutlineMode outline_mode( )
	//{
	//	if ( !this ) return EAresOutlineMode::None;
	//	return kernel->read<EAresOutlineMode>( this + offsets::outline_mod );
	//}

	//void outline_mode( EAresOutlineMode mode )
	//{
	//	if ( !this ) return;
	//	return kernel->write( this + offsets::outline_mod, mode );
	//}

	//std::tuple<TArray<USceneComponent*>*, int> attach_children( )
	//{
	//	return std::make_tuple(
	//		kernel->read<TArray<USceneComponent*>*>( this + offsets::attach_children ),
	//		kernel->read<int>( this + offsets::attach_children_count )
	//	);
	//}

	//Vector3 relative_scale_3d()
	//{
	//	if ( !this ) return Vector3::Zero( );
	//	return kernel->read<Vector3>(this + offsets::rel);
	//}

	Vector3 position( )
	{
		if ( !this ) return Vector3::Zero( );
		return kernel->read<Vector3>( this + offsets::relative_location);
	}
};

class USkeletalMeshComponent : public USceneComponent
{
public:
	std::wstring cvisible( )
	{
		//if ( !this ) return false;
		const auto chr = kernel->read<char>( this + offsets::is_visible );

		const std::wstring wstr = std::to_wstring( chr ).c_str( );
		return wstr;

		//return wstr == _( L"82" ) && wstr != _( L"18" );
	}

	bool visible( )
	{
		if ( !this ) return false;
		const auto chr = kernel->read<char>( this + offsets::is_visible );

		const std::wstring wstr = std::to_wstring( chr ).c_str( );

		return wstr == _( L"82" ) && wstr != _( L"18" );
	}

	enum class Bone : int
	{
		Skeleton = 0,
		Root = 1,

		MaleFemaleRThigh1 = 82,
		MaleFemaleRThigh2 = 83,
		MaleFemaleRFoot = 88,
		MaleFemaleThigh1 = 75,
		MaleFemaleLThigh2 = 76,
		MaleFemaleLFoot = 81,
		MaleFemaleLUpperarm = 47,
		MaleFemaleLForearm1 = 42,
		MaleFemaleLForearm2 = 43,
		MaleFemaleRUpperarm = 73,
		MaleFemaleRForearm1 = 68,
		MaleFemaleRForearm2 = 69,
		MaleFemaleNeck = 21,

		FemaleRFoot = 85,
		FemaleLFoot = 78,
		FemaleLUpperarm = 45,
		FemaleLForearm1 = 41,
		FemaleLForearm2 = 42,
		FemaleRUpperarm = 48,
		FemaleRForearm1 = 67,
		FemaleRForearm2 = 69,
		FemaleNeck = 21,

		DummyRThigh1 = 77,
		DummyRThigh2 = 79,
		DummyRFoot = 83,
		DummyLThigh1 = 63,
		DummyLThigh2 = 65,
		DummyLFoot = 69,
		DummyLUpperarm = 33,
		DummyLForearm1 = 30,
		DummyLForearm2 = 32,
		DummyRUpperarm = 58,
		DummyRForearm1 = 55,
		DummyRForearm2 = 57,
		DummyNeck = 9,

		GenericChest = 6,
		GenericStomach = 4,
		GenericPelvis = 3,
		GenericHead = 8,
	};

	enum class BodyType
	{
		Invalid,
		MaleFemale,
		Female,
		Bot
	};

	std::tuple<TArray<FTransform*>*, int> bones( )
	{
		if ( !this ) return std::make_tuple(nullptr, 0 );
		auto bone_array = kernel->read< TArray<FTransform*>* >( this + offsets::bone_array );
		if(!bone_array)
			bone_array = kernel->read< TArray<FTransform*>* >(this + offsets::bone_array + 0x10);

		return std::make_tuple(
			bone_array,
			kernel->read<int>( this + offsets::bone_count )
		);
	}

	box_bounds get_bounds( float expand = 5.0f )
	{
		if ( !this ) return box_bounds::null( );
		const auto [bone_array, bone_count] = this->bones( );
		if ( bone_count == 0 ) return box_bounds::null( );

		std::vector<Vector3> bound_bones {};
		/*for ( int i = 0; i < bone_count / 2; i++ )
		{
			Vector3 bone_pos = this->get_bone_position( i );
			if ( bone_pos == Vector3::Zero( ) ) continue;

			bound_bones.push_back( bone_pos );
		}*/
		switch ( bone_count )
		{
		case 104:
			bound_bones.push_back( this->get_bone_position( Bone::GenericHead ) + Vector3(0, 0, 15) );
			bound_bones.push_back( this->get_bone_position( Bone::MaleFemaleLFoot ) ); // L Foot
			bound_bones.push_back( this->get_bone_position( Bone::MaleFemaleRFoot ) ); // R Foot
			bound_bones.push_back( this->get_bone_position( Bone::MaleFemaleLForearm2 ) ); // L Hand
			bound_bones.push_back( this->get_bone_position( Bone::MaleFemaleRForearm2 ) ); // R Hand
			bound_bones.push_back( this->get_bone_position( Bone::MaleFemaleLUpperarm ) ); // L Upperarm
			bound_bones.push_back( this->get_bone_position( Bone::MaleFemaleRUpperarm ) ); // R Upperarm
			break;
		case 101:
			bound_bones.push_back( this->get_bone_position( Bone::GenericHead ) + Vector3( 0, 0, 15 ) );
			bound_bones.push_back( this->get_bone_position( Bone::FemaleLFoot ) ); // L Foot
			bound_bones.push_back( this->get_bone_position( Bone::FemaleRFoot ) ); // R Foot
			bound_bones.push_back( this->get_bone_position( Bone::FemaleLForearm2 ) ); // L Hand
			bound_bones.push_back( this->get_bone_position( Bone::FemaleRForearm2 ) ); // R Hand
			bound_bones.push_back( this->get_bone_position( Bone::FemaleLUpperarm ) ); // L Upperarm
			bound_bones.push_back( this->get_bone_position( Bone::FemaleRUpperarm ) ); // R Upperarm
			break;
		case 103:
			bound_bones.push_back( this->get_bone_position( Bone::GenericHead ) + Vector3( 0, 0, 15 ) );
			bound_bones.push_back( this->get_bone_position( Bone::DummyLThigh1 ) ); // L Foot
			bound_bones.push_back( this->get_bone_position( Bone::DummyRThigh2 ) ); // R Foot
			bound_bones.push_back( this->get_bone_position( Bone::DummyLFoot ) ); // L Foot
			bound_bones.push_back( this->get_bone_position( Bone::DummyRFoot ) ); // R Foot
			bound_bones.push_back( this->get_bone_position( Bone::DummyLForearm1 ) ); // L Hand
			bound_bones.push_back( this->get_bone_position( Bone::DummyRForearm1 ) ); // R Hand
			bound_bones.push_back( this->get_bone_position( Bone::DummyLForearm2 ) ); // L Hand
			bound_bones.push_back( this->get_bone_position( Bone::DummyRForearm2 ) ); // R Hand
			bound_bones.push_back( this->get_bone_position( Bone::DummyLUpperarm ) ); // L Upperarm
			bound_bones.push_back( this->get_bone_position( Bone::DummyRUpperarm ) ); // R Upperarm
			break;
		}

		if ( !bound_bones.size( ) ) return box_bounds::null( );

		box_bounds bounds = { FLT_MAX, FLT_MIN, FLT_MAX, FLT_MIN };
		for ( auto& bone_pos : bound_bones )
		{
			Vector2 screen_pos;
			if ( bone_pos == Vector3::Zero( ) ) continue;
			if ( camera_manager->world_to_screen( bone_pos, screen_pos ) )
			{
				if ( screen_pos == Vector2::Zero( ) ) continue;
				if ( screen_pos.x < bounds.left )
					bounds.left = screen_pos.x;
				if ( screen_pos.x > bounds.right )
					bounds.right = screen_pos.x;
				if ( screen_pos.y < bounds.top )
					bounds.top = screen_pos.y;
				if ( screen_pos.y > bounds.bottom )
					bounds.bottom = screen_pos.y;
			}
		}

		bound_bones.clear();

		bounds.left -= expand;
		bounds.right += expand;
		bounds.top -= expand;
		bounds.bottom += expand;

		return bounds;
	}

	Vector3 get_bone_position( int idx )
	{
		if ( !this ) return Vector3::Zero( );
		const auto [bone_array, bone_count] = this->bones( );
		FTransform firstBone = kernel->read<FTransform>( bone_array + ( sizeof( FTransform ) * idx ) );
		FTransform componentToWorld = kernel->read<FTransform>( this + offsets::component_to_world );
		D3DMATRIX matrix = MatrixMultiplication( firstBone.ToMatrixWithScale( ), componentToWorld.ToMatrixWithScale( ) );
		return Vector3( matrix._41, matrix._42, matrix._43 );
	}

	Vector3 get_bone_position( Bone idx )
	{
		if ( !this ) return Vector3::Zero( );
		const auto [bone_array, bone_count] = this->bones( );
		FTransform firstBone = kernel->read<FTransform>( bone_array + ( sizeof( FTransform ) * static_cast<int>( idx ) ) );
		FTransform componentToWorld = kernel->read<FTransform>( this + offsets::component_to_world );
		D3DMATRIX matrix = MatrixMultiplication( firstBone.ToMatrixWithScale( ), componentToWorld.ToMatrixWithScale( ) );
		return Vector3( matrix._41, matrix._42, matrix._43 );
	}

	BodyType body_type( )
	{
		if ( !this ) return BodyType::Bot;
		BodyType type = BodyType::Bot;
		const auto [_, bone_count] = this->bones( );
		switch ( bone_count )
		{
		case 0:
			type = BodyType::Invalid;
			break;
		case 104:
			type = BodyType::MaleFemale;
			break;
		case 103:
			type = BodyType::Bot;
			break;
		default:
			type = BodyType::Female;
			break;
		}

		return type;
	}
};

class UAresOutlineComponent
{
public:
	void outline_mode( EAresOutlineMode mode )
	{
		if ( !this ) return;
		kernel->write( this + offsets::outline_mode, mode );
	}
	EAresOutlineMode outline_mode( )
	{
		if ( !this ) return EAresOutlineMode::None;
		return kernel->read< EAresOutlineMode>( this + offsets::outline_mode );
	}
};

class AActor
{
public:
	//bool can_jump( )
	//{
	//	if ( !this ) return false;
	//	return kernel->read<uint32_t>( this + offsets::jump_count ) == 0;
	//}

	UAresOutlineComponent* outline_component( )
	{
		if ( !this ) return nullptr;
		return kernel->read< UAresOutlineComponent*>( this + offsets::outline_component );
	}
	uint32_t id( )
	{
		if ( !this ) return 0;
		return kernel->read<uint32_t>( this + 0x18 );
	}

	uint64_t unique_id( )
	{
		if ( !this ) return 0;
		return kernel->read<uintptr_t>( this + offsets::fname_id);
	}

	std::wstring name()
	{
		std::wstring result = _(L"");

		if ( !this ) return result;

		switch ( this->id( ) /* 0x18 */ )
		{
			/* Agents */
		case 9113255:
			result = _( L"NPC" );
			break;
		case 3402015:
			result = _( L"Astra" );
			break;
		case 3631386:
			result = _( L"Breach" );
			break;
		case 3854111:
			result = _( L"Brim" );
			break;
		case 4088354:
			result = _( L"Chamber" );
			break;
		case 4346593:
			result = _( L"Cypher" );
			break;
		case 4589034:
			result = _( L"Fade" );
			break;
		case 5067767:
			result = _(L"Harbor");
			break;
		case 5293455:
			result = _( L"Jett" );
			break;
		case 5504440:
			result = _( L"KAY/O" );
			break;
		case 5747464:
			result = _( L"Killjoy" );
			break;
		case 5785583:
			result = _( L"Neon" );
			break;
		case 6024554:
			result = _( L"Omen" );
			break;
		case 6237952:
			result = _( L"Phoenix" );
			break;
		case 6472046:
			result = _( L"Raze" );
			break;
		case 6702207:
			result = _( L"Reyna" );
			break;
		case 7053422:
			result = _( L"Sage" );
			break;
		case 7253966:
			result = _( L"Skye" );
			break;
		case 2967206:
			result = _( L"Sova" );
			break;
		case 7500530:
			result = _( L"Viper" );
			break;
		case 7752664:
			result = _( L"Yoru" );
			break;
		default:
			result = _( L"none" );
			break;
		}

		return result;
	}
};

class AAresEquippable
{
public:
	std::uint32_t id( )
	{
		return kernel->read<std::uint32_t>( this + offsets::actor_id );
	}

	std::uint32_t fname_id()
	{
		return kernel->read<std::uint32_t>(this + offsets::fname_id);
	}

	std::wstring result = _(L"");
	std::wstring name( )
	{

		if ( !this ) return result;


		switch ( this->fname_id( ) )
		{
			/* Weapons */
		case 1884019:
			result = _( L"Knife" );
			break;
		case 14743123:
			result = _( L"Classic" ); // range 
			break;
		case 2040836:
			result = _( L"Classic" );
			break;
		case 2575009:
			result = _( L"Shorty" );
			break;
		case 2579129:
			result = _( L"Frenzy" );
			break;
		case 2577900:
			result = _( L"Ghost" );
			break;
		case 2576478:
			result = _( L"Sheriff" );
			break;
		case 2560367:
			result = _( L"Stinger" );
			break;
		case 2564499:
			result = _( L"Spectre" );
			break;
		case 2581078:
			result = _( L"Bucky" );
			break;
		case 2582187:
			result = _( L"Judge" );
			break;
		case 2583313:
			result = _( L"Bulldog" );
			break;
		case 2567153:
			result = _( L"Guardian" );
			break;
		case 1878070:
			result = _( L"Phantom" );
			break;
		case 1878076:
			result = _( L"Vandal" );
			break;
		case 2566165:
			result = _( L"Marshal" );
			break;
		case 2568416:
			result = _( L"Operator" );
			break;
		case 2585773:
			result = _( L"Ares" );
			break;
		case 2563070:
			result = _( L"Odin" );
			break;
		case 14761269:
			result = _( L"Spike" );
			break;
		case 14761292:
			result = _( L"Defuser" );
			break;
			/* Astra Abilities */
		case 8333983:
			result = _( L"Astral Form" );
			break;
		case 8207686:
			result = _( L"Gravity Well" );
			break;
		case 8333971:
			result = _( L"Nova Pulse" );
			break;
		case 8333956:
			result = _( L"Nebula" );
			break;
			/* Breach Abilities */
		case 5920223:
			result = _( L"Aftershock" );
			break;
		case 5920240:
			result = _( L"Flashpoint" );
			break;
		case 5920232:
			result = _( L"Fault line" );
			break;
		case 5920248:
			result = _( L"Breach Ult" );
			break;
			/* Brimstone Abilities */
		case 8212977:
			result = _( L"Combat Stim" );
			break;
		case 8212986:
			result = _( L"Molly" );
			break;
		case 8212964:
			result = _( L"Smokes" );
			break;
		case 8212997:
			result = _( L"Brim Ult" );
			break;
			/* Chamber Abilities */
		case 7209583:
			result = _( L"Trademark" );
			break;
		case 7210089:
			result = _( L"Headhunter" );
			break;
		case 7209591:
			result = _( L"Rendezvous" );
			break;
		case 7210096:
			result = _( L"Chamber OP" );
			break;
			/* Cypher Abilities */
		case 2744003:
			result = _( L"Trapwire" );
			break;
		case 2743994:
			result = _( L"Cage" );
			break;
		case 2744012:
			result = _( L"Spycam" );
			break;
			/* Fade Abilities */
		case 5917996:
			result = _( L"Prowler" );
			break;
		case 5918029:
			result = _( L"Seize" );
			break;
		case 5918018:
			result = _( L"Haunt" );
			break;
		case 5918008:
			result = _( L"Nightfall" );
			break;
			/* Harbor Abilities */
		case 8210852:
			result = _(L"Cascade"); //here
			break;
		case 8210866:
			result = _(L"Cove");
			break;
		case 8210859:
			result = _(L"High Tide");
			break;
		case 8210875:
			result = _(L"Reckoning");
			break;
			/* Jett Abilities */
		case 5707066:
			result = _( L"Cloudburst" ); //here
			break;
		case 5707093:
			result = _( L"Updraft" );
			break;
		case 5707102:
			result = _( L"Knives" );
			break;
			/* Kayo Abilities */
		case 2728075:
			result = _( L"Grenade" );
			break;
		case 2728066:
			result = _( L"Flash" );
			break;
		case 2728057:
			result = _( L"Zeropoint" );
			break;
		case 2728085:
			result = _( L"Kay/o Ult" );
			break;
			/* Killjoy Abilities */
		case 8204910:
			result = _( L"Nanoswarm" );
			break;
		case 8204931:
			result = _( L"Alarmbot" );
			break;
		case 8204923:
			result = _( L"Turret" );
			break;
		case 8204940:
			result = _( L"KJ Ult" );
			break;
			/* Neon Abilities */
		case 2740291:
			result = _( L"Fast lane" );
			break;
		case 2740313:
			result = _( L"Relay Bolt" );
			break;
		case 2740302:
			result = _( L"High Gear" );
			break;
		case 2740718:
			result = _( L"Neon Ult" );
			break;
			/* Omen Abilities */
		case 2730985:
			result = _( L"Teleport" );
			break;
		case 2730995:
			result = _( L"Paranoia" );
			break;
		case 2730977:
			result = _( L"Smoke" );
			break;
		case 2731006:
			result = _( L"Omen Ult" );
			break;
			/* Phoenix Abilities */
		case 2722206:
			result = _( L"Blaze" );
			break;
		case 2722194:
			result = _( L"Curveball" );
			break;
		case 2722182:
			result = _( L"Fireball" );
			break;
			/* Raze Abilities */
		case 5922014:
			result = _( L"Boombot" );
			break;
		case 5922022:
			result = _( L"Blast Pack" );
			break;
		case 5922004:
			result = _( L"Grenade" );
			break;
		case 5922030:
			result = _( L"Raze Ult" );
			break;
			/* Reyna Abilities */
		case 2725106:
			result = _( L"Leer" );
			break;
			/* Sage Abilities */
		case 2736650:
			result = _( L"Barrier Orb" );
			break;
		case 2736638:
			result = _( L"Slow Orb" );
			break;
		case 2736660:
			result = _( L"Healing Orb" );
			break;
		case 2736671:
			result = _( L"Resurrection" );
			break;
			/* Skye Abilities */
		case 9210194:
			result = _( L"Regrowth" );
			break;
		case 9210210:
			result = _( L"Trailblazer" );
			break;
		case 9210201:
			result = _( L"Guiding Light" );
			break;
		case 9210220:
			result = _( L"Seekers" );
			break;
			/* Sova Abilities */
		case 7455819:
			result = _( L"Owl Drone" );
			break;
		case 7455809:
			result = _( L"Shock Bolt" );
			break;
		case 7455828:
			result = _( L"Recon Bolt" );
			break;
		case 7455840:
			result = _( L"Sova Ult" );
			break;
			/* Viper Abilities */
		case 2733631:
			result = _( L"Snake Bite" );
			break;
		case 2733611:
			result = _( L"Poison Cloud" );
			break;
		case 2733621:
			result = _( L"Toxic Screen" );
			break;
		case 2733641:
			result = _( L"Viper Ult" );
			break;
			/* Yoru Abilities */
		case 2752952:
			result = _( L"Fakeout" );
			break;
		case 2752969:
			result = _( L"Blindside" );
			break;
		case 2752960:
			result = _( L"Gatecrash" );
			break;
		case 2752979:
			result = _( L"Yoru Ult" );
			break;
		default:
			result = _( L"none" );
			break;
		}

		return result;
	}
};

class UAresInventory
{
public:
	AAresEquippable* current_equippable()
	{
		if ( !this ) return nullptr;
		return kernel->read<AAresEquippable*>(this + offsets::current_equippable);
	}
};

class APawn : public AActor
{
public:

	Vector2 find_mpv_bone( )
	{
		if ( !this ) return Vector2::Zero( );
		Vector3 head_position = this->mesh( )->get_bone_position( USkeletalMeshComponent::Bone::GenericHead );
		Vector3 chest_position = this->mesh( )->get_bone_position( USkeletalMeshComponent::Bone::GenericChest );
		Vector3 pelvis_position = this->mesh( )->get_bone_position( USkeletalMeshComponent::Bone::GenericPelvis );
		Vector3 stomach_position = this->mesh( )->get_bone_position( USkeletalMeshComponent::Bone::GenericStomach );
		Vector2 head_screen;
		Vector2 chest_screen;
		Vector2 pelvis_screen;
		Vector2 stomach_screen;

		if( camera_manager->world_to_screen( head_position, head_screen ) && 
			camera_manager->world_to_screen( chest_position, chest_screen ) &&
			camera_manager->world_to_screen( pelvis_position, pelvis_screen ) &&
			camera_manager->world_to_screen( stomach_position, stomach_screen )
			)
		{
			Vector2 result = head_screen;

			if ( chest_screen.distance( renderer->screen_center ) <
				head_screen.distance( renderer->screen_center ) )
				result = chest_screen;

			if ( pelvis_screen.distance( renderer->screen_center ) <
				chest_screen.distance( renderer->screen_center ) )
				result = pelvis_screen;

			if ( stomach_screen.distance( renderer->screen_center ) <
				pelvis_screen.distance( renderer->screen_center ) )
				result = stomach_screen;

			return result;
		} 

		return Vector2::Zero( );
	}
	UDamageableComponent* damage_controller( )
	{
		if ( !this ) return nullptr;
		return kernel->read<UDamageableComponent*>( this + offsets::damage_handler);
	}

	USceneComponent* root_component( )
	{
		if ( !this ) return nullptr;
		return kernel->read<USceneComponent*>( this + offsets::root_component );
	}

	bool is_dormant( )
	{
		if ( !this ) return false;
		return kernel->read<bool>( this + offsets::dormant);
	}

	PlayerState* player_state( )
	{
		if ( !this ) return nullptr;
		return kernel->read<PlayerState*>( this + offsets::player_state );
	}

	USkeletalMeshComponent* mesh( )
	{
		if ( !this ) return nullptr;
		return kernel->read<USkeletalMeshComponent*>( this + offsets::current_mesh );
	}

	UAresInventory* inventory()
	{
		if ( !this ) return nullptr;
		return kernel->read<UAresInventory*>( this + offsets::inventory );
	}

	//void outline( EAresOutlineMode mode )
	//{
	//	if ( !this ) return;

	//	const auto mesh = this->mesh( );

	//	if ( mesh->outline_mode( ) != mode )
	//	{
	//		mesh->outline_mode( mode );

	//		auto [children, count] = mesh->attach_children( );

	//		for ( int i = 0; i < count; i++ ) {
	//			if ( USceneComponent* current_mesh = children->get( i ) )
	//			{
	//				if ( current_mesh->outline_mode( ) == EAresOutlineMode::Enemy || current_mesh->outline_mode( ) == EAresOutlineMode::Outline && mode != EAresOutlineMode::Outline ) {
	//					current_mesh->outline_mode( mode );
	//				}
	//			}
	//		}
	//	}
	//}
};

class APlayerController
{
public:
	APlayerCameraManager* camera_manager( )
	{
		if ( !this ) return nullptr;
		return kernel->read<APlayerCameraManager*>( this + offsets::camera_controller );
	}

	APawn* acknowledged_pawn( )
	{
		if ( !this ) return nullptr;
		return kernel->read<APawn*>( this + offsets::acknowledged_pawn );
	}

	Vector3 ControlRotation( )
	{
		if ( !this ) return Vector3::Zero( );
		return kernel->read<Vector3>( this + offsets::control_rotation );
	}

	void ControlRotation( Vector3 target )
	{
		if ( !this ) return;
		kernel->write<Vector3>( this + offsets::control_rotation, target );
	}
};

class ULocalPlayer
{
public:
	APlayerController* controller( )
	{
		if ( !this ) return nullptr;
		return kernel->read<APlayerController*>( this + offsets::player_controller );
	}
};


class ULevel
{
public:
	std::tuple<TArray<APawn*>*, int> actors( )
	{
		return std::make_tuple(
			kernel->read<TArray<APawn*>*>( this + offsets::actor_array ),
			kernel->read<int>( this + offsets::actor_count )
		);
	}
};

class UGameInstance
{
public:
	ULocalPlayer* localplayer( )
	{
		uintptr_t local_player_array = kernel->read( this + offsets::local_players );
		return kernel->read<ULocalPlayer*>( local_player_array );
	}
};

class UWorld
{
private:
	__forceinline static __int64 decrypt_uworld( const uint32_t key, const uintptr_t* state )
	{

		unsigned __int64 v19; // r11
		unsigned __int64 v20; // r8
		unsigned __int64 v21; // r9
		unsigned int v22; // er10
		unsigned __int64 v23; // rcx
		unsigned __int64 v24; // rdx
		unsigned __int64 v25; // rcx
		int v26; // ebx
		unsigned int v27; // ecx
		__int64 v28; // rax
		unsigned __int64 v29; // r8
		unsigned __int64 v30; // r8
		unsigned __int64 v31; // rcx
		unsigned __int64 v32; // rdx
		unsigned __int64 v33; // rcx

		v19 = 2685821657736338717i64
			* ( (unsigned int) key ^ (unsigned int) ( key << 25 ) ^ ( ( (unsigned int) key ^ ( (unsigned __int64) (unsigned int) key >> 15 ) ) >> 12 ) )
			% 7;
		v20 = state[ v19 ];
		v21 = ( 2685821657736338717i64
			* ( (unsigned int) key ^ (unsigned int) ( key << 25 ) ^ ( ( (unsigned int) key ^ ( (unsigned __int64) (unsigned int) key >> 15 ) ) >> 12 ) ) ) >> 32;
		v22 = (unsigned int) v19 % 7;
		if ( !( (unsigned int) v19 % 7 ) )
		{
			v23 = ( 2 * ( v20 - (unsigned int) ( v21 - 1 ) ) ) ^ ( ( 2 * ( v20 - (unsigned int) ( v21 - 1 ) ) ) ^ ( ( v20
				- (unsigned int) ( v21 - 1 ) ) >> 1 ) ) & 0x5555555555555555i64;
			v24 = ( 4 * v23 ) ^ ( ( 4 * v23 ) ^ ( v23 >> 2 ) ) & 0x3333333333333333i64;
			v25 = ( 16 * v24 ) ^ ( ( 16 * v24 ) ^ ( v24 >> 4 ) ) & 0xF0F0F0F0F0F0F0Fi64;
			v20 = __ROL8__( ( v25 << 8 ) ^ ( ( v25 << 8 ) ^ ( v25 >> 8 ) ) & 0xFF00FF00FF00FFi64, 32 );
		LABEL_26:
			v26 = 2 * v19;
			goto LABEL_27;
		}
		if ( v22 != 1 )
			goto LABEL_26;
		v26 = 2 * v19;
		v20 = __ROL8__( v20 - (unsigned int) ( 2 * v19 + v21 ), (unsigned __int8) ( ( (int) v21 + (int) v19 ) % 0x3Fu ) + 1 );
	LABEL_27:
		v27 = v26 + v21;
		if ( v22 == 2 )
			v20 = ~( v20 - v27 );
		switch ( v22 )
		{
		case 3u:
			v28 = 2 * ( ( 2 * v20 ) ^ ( ( 2 * v20 ) ^ ( v20 >> 1 ) ) & 0x5555555555555555i64 );
			v20 = v28 ^ ( v28 ^ ( ( ( 2 * v20 ) ^ ( ( 2 * v20 ) ^ ( v20 >> 1 ) ) & 0x5555555555555555i64 ) >> 1 ) ) & 0x5555555555555555i64;
			break;
		case 4u:
			v29 = __ROR8__( v20, (unsigned __int8) ( v27 % 0x3F ) + 1 );
			v20 = ( 2 * v29 ) ^ ( ( 2 * v29 ) ^ ( v29 >> 1 ) ) & 0x5555555555555555i64;
			break;
		case 5u:
			v30 = __ROR8__( v20, (unsigned __int8) ( v27 % 0x3F ) + 1 );
			v31 = ( 2 * v30 ) ^ ( ( 2 * v30 ) ^ ( v30 >> 1 ) ) & 0x5555555555555555i64;
			v32 = ( 4 * v31 ) ^ ( ( 4 * v31 ) ^ ( v31 >> 2 ) ) & 0x3333333333333333i64;
			v33 = ( 16 * v32 ) ^ ( ( 16 * v32 ) ^ ( v32 >> 4 ) ) & 0xF0F0F0F0F0F0F0Fi64;
			v20 = __ROL8__( ( v33 << 8 ) ^ ( ( v33 << 8 ) ^ ( v33 >> 8 ) ) & 0xFF00FF00FF00FFi64, 32 );
			break;
		case 6u:
			v20 = ~v20 - (unsigned int) ( v21 + v19 );
			break;
		}
		return v20 ^ (unsigned int) key;
	}
public:
	ULevel* persistent_level( )
	{
		return kernel->read<ULevel*>( this + offsets::persistent_level );
	}

	UGameInstance* game_instance( )
	{
		return kernel->read<UGameInstance*>( this + offsets::owning_game_instance );
	}

	static UWorld* Instance( )
	{

		//auto key = kernel->read( kernel->base_address + offsets::uworld_key );
		//auto state = kernel->read<State>( kernel->base_address + offsets::uworld_state );
		//auto decrypted_ptr = decrypt_uworld( key, (uintptr_t*) &state );
		return kernel->read<UWorld*>(kernel->guard_address + 0x60);
	}
	//static void ally_outline_color( Color3 color ) // prolly doesn't belong here
	//{
	//	kernel->write( kernel->base_address + offsets::outline_ally_color, color );
	//}
	//static Color3 ally_outline_color( )
	//{
	//	return kernel->read<Color3>( kernel->base_address + offsets::outline_ally_color );
	//}
	//static void enemy_outline_color( Color3 color )
	//{
	//	kernel->write( kernel->base_address + offsets::outline_enemy_color, color );
	//}
	//static Color3 enemy_outline_color( )
	//{
	//	return kernel->read<Color3>( kernel->base_address + offsets::outline_enemy_color );
	//}
};