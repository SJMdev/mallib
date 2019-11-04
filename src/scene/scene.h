#ifndef INCLUDED_SCENE_
#define INCLUDED_SCENE_

#include <string>

namespace scene
{
	struct Set_Piece
	{
		std::string name;
		std::string model_name;
		std::string material_name;
		std::string texture_name;
		XForm_state xform_state;
	}

	struct Scene
	{
		std::vector<Set_Piece> set_pieces;
	}
}



#endif // INCLUDED_SCENE