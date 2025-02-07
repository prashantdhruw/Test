#include "backend/looped/looped.hpp"
#include "backend/looped_command.hpp"
#include "fiber_pool.hpp"
#include "gta/enums.hpp"
#include "natives.hpp"
#include "util/math.hpp"

namespace big
{
	class preview_cam : looped_command
	{
		using looped_command::looped_command;

		int cameraAngle = 0;

		virtual void on_enable() override
		{
			CAM::SET_FOLLOW_PED_CAM_VIEW_MODE(2);
			ENTITY::FREEZE_ENTITY_POSITION(self::ped, true);
		}

		virtual void on_tick() override
		{
			PAD::DISABLE_CONTROL_ACTION(0, 30, 1);
			PAD::DISABLE_CONTROL_ACTION(0, 31, 1);
			PAD::DISABLE_CONTROL_ACTION(0, 32, 1);
			PAD::DISABLE_CONTROL_ACTION(0, 33, 1);
			PAD::DISABLE_CONTROL_ACTION(0, 34, 1);
			PAD::DISABLE_CONTROL_ACTION(0, 35, 1);
			PAD::DISABLE_CONTROL_ACTION(0, 1, 1);
			PAD::DISABLE_CONTROL_ACTION(0, 2, 1);

			CAM::INVALIDATE_IDLE_CAM();

			if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, 35))
			{
				cameraAngle += 90;
				if (cameraAngle >= 360)
				{
					cameraAngle = 0;
				}
			}
			else if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, 34))
			{
				cameraAngle -= 90;
				if (cameraAngle < 0)
				{
					cameraAngle = 360;
				}
			}
			CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(static_cast<float>(cameraAngle));
		}


		virtual void on_disable() override
		{
			CAM::SET_FOLLOW_PED_CAM_VIEW_MODE(2);
			CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);

			ENTITY::FREEZE_ENTITY_POSITION(self::ped, false);
		}
	};

	preview_cam g_preview_cam("previewcam", "PREVIEWCAM", "PREVIEWCAM_DESC", g.self.preview_cam);
}
