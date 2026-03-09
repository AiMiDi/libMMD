//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "VMDFile.h"

#include <libMMD/Base/Log.h>
#include <libMMD/Base/File.h>

namespace libmmd
{
	namespace
	{
		template <typename T>
		bool Read(T* val, MemoryReader& file)
		{
			return file.Read(val);
		}

		template <typename T, typename Writer>
		bool Write(const T* val, Writer& file)
		{
			return file.Write(val);
		}

		bool ReadHeader(VMDFile* vmd, MemoryReader& file)
		{
			Read(&vmd->m_header.m_header, file);
			Read(&vmd->m_header.m_modelName, file);

			if (vmd->m_header.m_header.ToString() != "Vocaloid Motion Data 0002" &&
				vmd->m_header.m_header.ToString() != "Vocaloid Motion Data"
				)
			{
				LIBMMD_WARN("VMD Header error.");
				return false;
			}

			return !file.IsBad();
		}

		template <typename Writer>
		bool WriteHeader(const VMDFile* vmd, Writer& file)
		{
			Write(vmd->m_header.m_header, file);
			Write(vmd->m_header.m_modelName, file);
			return !file.IsBad();
		}

		bool ReadMotion(VMDFile* vmd, MemoryReader& file)
		{
			uint32_t motionCount = 0;
			if (!Read(&motionCount, file))
			{
				return false;
			}

			vmd->m_motions.resize(motionCount);
			for (auto& [m_boneName, m_frame, m_translate, m_quaternion, m_interpolation] : vmd->m_motions)
			{
				Read(&m_boneName, file);
				Read(&m_frame, file);
				Read(&m_translate, file);
				Read(&m_quaternion, file);
				Read(&m_interpolation, file);
			}

			return !file.IsBad();
		}

		template <typename Writer>
		bool WriteMotion(const VMDFile* vmd, Writer& file)
		{
			const auto motionCount = static_cast<uint32_t>(vmd->m_motions.size());
			if (!Write(&motionCount, file))
			{
				return false;
			}

			for (const auto& motion : vmd->m_motions)
			{
				Write(motion.m_boneName, file);
				Write(&motion.m_frame, file);
				Write(&motion.m_translate, file);
				Write(&motion.m_quaternion, file);
				Write(&motion.m_interpolation, file);
			}

			return !file.IsBad();
		}

		bool ReadBlendShape(VMDFile* vmd, MemoryReader& file)
		{
			uint32_t blendShapeCount = 0;
			if (!Read(&blendShapeCount, file))
			{
				return false;
			}

			vmd->m_morphs.resize(blendShapeCount);
			for (auto& [m_blendShapeName, m_frame, m_weight] : vmd->m_morphs)
			{
				Read(&m_blendShapeName, file);
				Read(&m_frame, file);
				Read(&m_weight, file);
			}

			return !file.IsBad();
		}

		template <typename Writer>
		bool WriteBlendShape(const VMDFile* vmd, Writer& file)
		{
			const auto blendShapeCount = static_cast<uint32_t>(vmd->m_morphs.size());
			if (!Write(&blendShapeCount, file))
			{
				return false;
			}

			for (const auto& morph : vmd->m_morphs)
			{
				Write(morph.m_blendShapeName, file);
				Write(&morph.m_frame, file);
				Write(&morph.m_weight, file);
			}

			return !file.IsBad();
		}

		bool ReadCamera(VMDFile* vmd, MemoryReader& file)
		{
			uint32_t cameraCount = 0;
			if (!Read(&cameraCount, file))
			{
				return false;
			}

			vmd->m_cameras.resize(cameraCount);
			for (auto& [m_frame, m_distance, m_interest, m_rotate, m_interpolation, m_viewAngle, m_isPerspective] : vmd->m_cameras)
			{
				Read(&m_frame, file);
				Read(&m_distance, file);
				Read(&m_interest, file);
				Read(&m_rotate, file);
				Read(&m_interpolation, file);
				Read(&m_viewAngle, file);
				Read(&m_isPerspective, file);
			}

			return !file.IsBad();
		}

		template <typename Writer>
		bool WriteCamera(const VMDFile* vmd, Writer& file)
		{
			const auto cameraCount = static_cast<uint32_t>(vmd->m_cameras.size());
			if (!Write(&cameraCount, file))
			{
				return false;
			}

			for (const auto& camera : vmd->m_cameras)
			{
				Write(&camera.m_frame, file);
				Write(&camera.m_distance, file);
				Write(&camera.m_interest, file);
				Write(&camera.m_rotate, file);
				Write(&camera.m_interpolation, file);
				Write(&camera.m_viewAngle, file);
				Write(&camera.m_isPerspective, file);
			}

			return !file.IsBad();
		}

		bool ReadLight(VMDFile* vmd, MemoryReader& file)
		{
			uint32_t lightCount = 0;
			if (!Read(&lightCount, file))
			{
				return false;
			}

			vmd->m_lights.resize(lightCount);
			for (auto& [m_frame, m_color, m_position] : vmd->m_lights)
			{
				Read(&m_frame, file);
				Read(&m_color, file);
				Read(&m_position, file);
			}

			return !file.IsBad();
		}

		template <typename Writer>
		bool WriteLight(const VMDFile* vmd, Writer& file)
		{
			const auto lightCount = static_cast<uint32_t>(vmd->m_lights.size());
			if (!Write(&lightCount, file))
			{
				return false;
			}

			for (const auto& light : vmd->m_lights)
			{
				Write(&light.m_frame, file);
				Write(&light.m_color, file);
				Write(&light.m_position, file);
			}

			return !file.IsBad();
		}

		bool ReadShadow(VMDFile* vmd, MemoryReader& file)
		{
			uint32_t shadowCount = 0;
			if (!Read(&shadowCount, file))
			{
				return false;
			}

			vmd->m_shadows.resize(shadowCount);
			for (auto& [m_frame, m_shadowType, m_distance] : vmd->m_shadows)
			{
				Read(&m_frame, file);
				Read(&m_shadowType, file);
				Read(&m_distance, file);
			}

			return !file.IsBad();
		}

		template <typename Writer>
		bool WriteShadow(const VMDFile* vmd, Writer& file)
		{
			const auto shadowCount = static_cast<uint32_t>(vmd->m_shadows.size());
			if (!Write(&shadowCount, file))
			{
				return false;
			}

			for (const auto& shadow : vmd->m_shadows)
			{
				Write(&shadow.m_frame, file);
				Write(&shadow.m_shadowType, file);
				Write(&shadow.m_distance, file);
			}

			return !file.IsBad();
		}

		bool ReadIK(VMDFile* vmd, MemoryReader& file)
		{
			uint32_t ikCount = 0;
			if (!Read(&ikCount, file))
			{
				return false;
			}

			vmd->m_iks.resize(ikCount);
			for (auto& [m_frame, m_show, m_ikInfos] : vmd->m_iks)
			{
				Read(&m_frame, file);
				Read(&m_show, file);
				uint32_t ikInfoCount = 0;
				if (!Read(&ikInfoCount, file))
				{
					return false;
				}
				m_ikInfos.resize(ikInfoCount);
				for (auto& [m_name, m_enable] : m_ikInfos)
				{
					Read(&m_name, file);
					Read(&m_enable, file);
				}
			}

			return !file.IsBad();
		}

		template <typename Writer>
		bool WriteIK(const VMDFile* vmd, Writer& file)
		{
			const auto ikCount = static_cast<uint32_t>(vmd->m_iks.size());
			if (!Write(&ikCount, file))
			{
				return false;
			}

			for (const auto& ik : vmd->m_iks)
			{
				Write(&ik.m_frame, file);
				Write(&ik.m_show, file);
				const auto ikInfoCount = static_cast<uint32_t>(ik.m_ikInfos.size());
				if (!Write(&ikInfoCount, file))
				{
					return false;
				}
				for (const auto& ikInfo : ik.m_ikInfos)
				{
					Write(ikInfo.m_name, file);
					Write(&ikInfo.m_enable, file);
				}
			}

			return !file.IsBad();
		}

		bool ReadVMDFile(VMDFile* vmd, MemoryReader& file)
		{
			if (!ReadHeader(vmd, file))
			{
				LIBMMD_WARN("ReadHeader Fail.");
				return false;
			}

			if (!ReadMotion(vmd, file))
			{
				LIBMMD_WARN("ReadMotion Fail.");
				return false;
			}

			if (file.Tell() < file.GetSize())
			{
				if (!ReadBlendShape(vmd, file))
				{
					LIBMMD_WARN("ReadBlednShape Fail.");
					return false;
				}
			}

			if (file.Tell() < file.GetSize())
			{
				if (!ReadCamera(vmd, file))
				{
					LIBMMD_WARN("ReadCamera Fail.");
					return false;
				}
			}

			if (file.Tell() < file.GetSize())
			{
				if (!ReadLight(vmd, file))
				{
					LIBMMD_WARN("ReadLight Fail.");
					return false;
				}
			}

			if (file.Tell() < file.GetSize())
			{
				if (!ReadShadow(vmd, file))
				{
					LIBMMD_WARN("ReadShadow Fail.");
					return false;
				}
			}

			if (file.Tell() < file.GetSize())
			{
				if (!ReadIK(vmd, file))
				{
					LIBMMD_WARN("ReadIK Fail.");
					return false;
				}
			}

			return true;
		}

		template <typename Writer>
		bool WriteVMDFile(const VMDFile* vmd, Writer& file)
		{
			if (!WriteHeader(vmd, file))
			{
				LIBMMD_WARN("WriteHeader Fail.");
				return false;
			}

			if (!WriteMotion(vmd, file))
			{
				LIBMMD_WARN("WriteMotion Fail.");
				return false;
			}

			if (!WriteBlendShape(vmd, file))
			{
				LIBMMD_WARN("WriteBlendShape Fail.");
				return false;
			}

			if (!WriteCamera(vmd, file))
			{
				LIBMMD_WARN("WriteCamera Fail.");
				return false;
			}

			if (!WriteLight(vmd, file))
			{
				LIBMMD_WARN("WriteLight Fail.");
				return false;
			}

			if (!WriteShadow(vmd, file))
			{
				LIBMMD_WARN("WriteShadow Fail.");
				return false;
			}

			if (!WriteIK(vmd, file))
			{
				LIBMMD_WARN("WriteIK Fail.");
				return false;
			}

			return true;
		}
	}

	bool ReadVMDFile(VMDFile * vmd, const char * filename)
	{
		File file;
		if (!file.Open(filename))
		{
			LIBMMD_WARN("VMD File Open Fail. {}", filename);
			return false;
		}

		std::vector<uint8_t> buffer;
		if (!file.ReadAll(&buffer))
		{
			LIBMMD_WARN("VMD File Read Fail. {}", filename);
			return false;
		}
		file.Close();

		MemoryReader reader(buffer.data(), buffer.size());
		return ReadVMDFile(vmd, reader);
	}

	bool ReadVMDFile(VMDFile* vmd, const uint8_t* data, size_t size)
	{
		MemoryReader reader(data, size);
		return ReadVMDFile(vmd, reader);
	}

	bool WriteVMDFile(const VMDFile* vmd, const char* filename)
	{
		File file;
		if (!file.Create(filename))
		{
			LIBMMD_WARN("VMD File Open Fail. {}", filename);
			return false;
		}

		return WriteVMDFile(vmd, file);
	}

	bool WriteVMDFile(const VMDFile* vmd, std::vector<uint8_t>& outData)
	{
		MemoryWriter writer;
		if (!WriteVMDFile(vmd, writer))
		{
			return false;
		}
		outData = std::move(writer.GetData());
		return true;
	}

}
