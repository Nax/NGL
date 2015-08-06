/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*    SkeletonPose.hpp                               oooooo       oooooo      */
/*                                                 oooooooooo   oooooooooo    */
/*                                                         o%%%%%o            */
/*                                                         %:::::%            */
/*                                                        %:::::::%           */
/*    This file is part of the                             %:::::%            */
/*    Lums library.                                         %%%%%             */
/*                                                                            */
/* ************************************************************************** */

#ifndef LUMS_SKELETON_POSE_HPP
#define LUMS_SKELETON_POSE_HPP

#include <fstream>
#include <vector>
#include <LumsInclude/Provider.hpp>
#include <LumsInclude/Skeleton/Bone.hpp>
#include <LumsInclude/Skeleton/Skin.hpp>
#include <LumsInclude/Skeleton/SkeletonIK.hpp>

namespace lm
{
    class SkeletonPose
    {
    public:
    	SkeletonPose();
        
        void        loadFromFile(std::ifstream& file);

        void        update();
        
        const std::vector<Bone>&        bones() const { return _bones; }
        const std::vector<Skin>&        skins() const { return _skins; }
        const std::vector<SkeletonIK>&  iks() const { return _iks; }

    protected:
        std::vector<Bone>       _bones;
        std::vector<Skin>       _skins;
        std::vector<SkeletonIK> _iks;
    };
}

#endif
