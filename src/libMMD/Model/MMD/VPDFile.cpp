//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "VPDFile.h"

#include <algorithm>
#include <cstdio>
#include <string>

#include <libMMD/Base/Log.h>
#include <libMMD/Base/File.h>
#include <libMMD/Base/UnicodeUtil.h>

#include "SjisToUnicode.h"

namespace libmmd
{
	bool ReadVPDFile(VPDFile * vpd, const char * filename)
	{
		
		TextFileReader file;
		if (!file.Open(filename))
		{
			LIBMMD_INFO("VPD File Open Fail. {}", filename);
			return false;
		}

		std::vector<std::string> lines;
		file.ReadAllLines(lines);

		if (lines.empty() || lines[0] != "Vocaloid Pose Data file")
		{
			LIBMMD_INFO("VPD File Format Error.");
			return false;
		}

		// remove comment
		for (auto it = lines.begin() + 1; it != lines.end(); ++it)
		{
			auto& line = *it;
			auto commentPos = line.find("//");
			line = line.substr(0, commentPos);
		}

		auto lineIt = lines.begin() + 1;
		// parent file name
		lineIt = std::find_if(lineIt, lines.end(), [](const std::string& line) {
			return !line.empty();
		});
		if (lineIt == lines.end())
		{
			LIBMMD_INFO("VPD File Parse Error.[parent fil name]");
			return false;
		}
		++lineIt;

		// num bones
		lineIt = std::find_if(lineIt, lines.end(), [](const std::string& line) {
			return !line.empty();
		});
		if (lineIt == lines.end())
		{
			LIBMMD_INFO("VPD File Parse Error.[num bones]");
			return false;
		}
		int numBones;
		try
		{
			const auto& line = *lineIt;
			auto numStr = line.substr(0, line.find(';'));
			numBones = std::stoi(numStr);
		}
		catch (std::exception& e)
		{
			LIBMMD_INFO("VPD File Parse Error. {}:[{}]", static_cast<size_t>(lineIt - lines.begin()), e.what());
			return false;
		}
		std::vector<VPDBone> bones(numBones);

		++lineIt;

		lineIt = std::find_if(lineIt, lines.end(), [](const std::string& line) {
			return !line.empty();
		});
		int boneCount = 0;
		while (boneCount < numBones && lineIt != lines.end())
		{
			int boneIdx;
			{
				const auto& line = *lineIt;
				auto delimPos1 = line.find("Bone");
				if (delimPos1 == std::string::npos)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Not Found Bone]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}
				delimPos1 += 4;

				auto delimPos2 = line.find('{', delimPos1);
				if (delimPos2 == std::string::npos)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Not Found Bone]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}

				auto numStr = line.substr(delimPos1, delimPos2 - delimPos1);
				try
				{
					boneIdx = std::stoi(numStr);
				}
				catch (std::exception& e)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[{}]", static_cast<size_t>(lineIt - lines.begin()), e.what());
					return false;
				}
				if (boneIdx >= numBones)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Bone Index over]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}
				bones[boneIdx].m_boneName = line.substr(delimPos2 + 1);
			}
			++lineIt;
			lineIt = std::find_if(lineIt, lines.end(), [](const std::string& line) {
				return !line.empty();
			});
			
			{
				const auto& line = *lineIt;
				auto delim1 = line.find_first_not_of(" \t");
				auto delim2 = line.find_first_of(" \t,", delim1);
				if (std::string::npos == delim1 || std::string::npos == delim2)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Split error]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}

				auto delim3 = line.find_first_not_of(" \t,", delim2);
				auto delim4 = line.find_first_of(" \t,", delim3);
				if (std::string::npos == delim3 || std::string::npos == delim4)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Split error]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}

				auto delim5 = line.find_first_not_of(" \t,", delim4);
				auto delim6 = line.find_first_of(" \t;", delim5);
				if (std::string::npos == delim5 || std::string::npos == delim6)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Split error]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}

				if (auto delim7 = line.find_first_of(';', delim5); std::string::npos == delim7)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Split error]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}

				try
				{
					auto numStr1 = line.substr(delim1, delim2 - delim1);
					auto numStr2 = line.substr(delim3, delim4 - delim3);
					auto numStr3 = line.substr(delim5, delim6 - delim5);
				bones[boneIdx].m_translate.x() = std::stof(numStr1);
				bones[boneIdx].m_translate.y() = std::stof(numStr2);
				bones[boneIdx].m_translate.z() = std::stof(numStr3);
				}
				catch (std::exception& e)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[{}]", static_cast<size_t>(lineIt - lines.begin()), e.what());
					return false;
				}
			}
			++lineIt;
			lineIt = std::find_if(lineIt, lines.end(), [](const std::string& line) {
				return !line.empty();
			});

			{
				const auto& line = *lineIt;
				auto delim1 = line.find_first_not_of(" \t");
				auto delim2 = line.find_first_of(" \t,", delim1);
				if (std::string::npos == delim1 || std::string::npos == delim2)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Split error]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}

				auto delim3 = line.find_first_not_of(" \t,", delim2);
				auto delim4 = line.find_first_of(" \t,", delim3);
				if (std::string::npos == delim3 || std::string::npos == delim4)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Split error]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}

				auto delim5 = line.find_first_not_of(" \t,", delim4);
				auto delim6 = line.find_first_of(" \t,", delim5);
				if (std::string::npos == delim5 || std::string::npos == delim6)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Split error]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}

				auto delim7 = line.find_first_not_of(" \t,", delim6);
				auto delim8 = line.find_first_of(" \t;", delim7);
				if (std::string::npos == delim7 || std::string::npos == delim8)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Split error]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}

				if (std::string::npos == delim7)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Split error]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}

				try
				{
					auto numStr1 = line.substr(delim1, delim2 - delim1);
					auto numStr2 = line.substr(delim3, delim4 - delim3);
					auto numStr3 = line.substr(delim5, delim6 - delim5);
					auto numStr4 = line.substr(delim7, delim8 - delim7);
				bones[boneIdx].m_quaternion.x() = std::stof(numStr1);
				bones[boneIdx].m_quaternion.y() = std::stof(numStr2);
				bones[boneIdx].m_quaternion.z() = std::stof(numStr3);
				bones[boneIdx].m_quaternion.w() = std::stof(numStr4);
				}
				catch (std::exception& e)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[{}]", static_cast<size_t>(lineIt - lines.begin()), e.what());
					return false;
				}
			}
			++lineIt;
			lineIt = std::find_if(lineIt, lines.end(), [](const std::string& line) {
				return !line.empty();
			});

			{
				if (lineIt->find('}') == std::string::npos)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Split error]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}
			}
			++lineIt;
			lineIt = std::find_if(lineIt, lines.end(), [](const std::string& line) {
				return !line.empty();
			});
			boneCount++;
		}

		for (auto& bone : bones)
		{
			std::u16string u16Str = ConvertSjisToU16String(bone.m_boneName.c_str());
			std::string u8Str;
			ConvU16ToU8(u16Str, u8Str);
			bone.m_boneName = u8Str;
		}

		vpd->m_bones = std::move(bones);

		std::vector<VPDMorph> morphs;
		while (lineIt != lines.end())
		{
			VPDMorph morph;
			{
				const auto& line = *lineIt;
				auto delimPos1 = line.find("Morph");
				if (delimPos1 == std::string::npos)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Not Found Morph]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}
				delimPos1 += sizeof("Morph") - 1;

				auto delimPos2 = line.find('{', delimPos1);
				if (delimPos2 == std::string::npos)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Not Found Morph]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}

				auto numStr = line.substr(delimPos1, delimPos2 - delimPos1);
				try
				{
					std::ignore = std::stoi(numStr);
				}
				catch (std::exception& e)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[{}]", static_cast<size_t>(lineIt - lines.begin()), e.what());
					return false;
				}
				morph.m_morphName = line.substr(delimPos2 + 1);
			}
			++lineIt;
			lineIt = std::find_if(lineIt, lines.end(), [](const std::string& line) {
				return !line.empty();
			});

			{
				const auto& line = *lineIt;
				auto delim1 = line.find_first_not_of(" \t");
				auto delim2 = line.find_first_of(" \t;", delim1);
				if (std::string::npos == delim1 || std::string::npos == delim2)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Split error]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}

				if (std::string::npos == delim2)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[Split error]", static_cast<size_t>(lineIt - lines.begin()));
					return false;
				}

				try
				{
					auto numStr1 = line.substr(delim1, delim2 - delim1);
					morph.m_weight = std::stof(numStr1);
				}
				catch (std::exception& e)
				{
					LIBMMD_INFO("VPD File Parse Error. {}:[{}]", static_cast<size_t>(lineIt - lines.begin()), e.what());
					return false;
				}
				++lineIt;
				lineIt = std::find_if(lineIt, lines.end(), [](const std::string& line2) {
					return !line2.empty();
				});

				{
					if (lineIt->find('}') == std::string::npos)
					{
						LIBMMD_INFO("VPD File Parse Error. {}:[Split error]", static_cast<size_t>(lineIt - lines.begin()));
						return false;
					}
				}
				++lineIt;
				lineIt = std::find_if(lineIt, lines.end(), [](const std::string& line2) {
					return !line2.empty();
				});
				boneCount++;
			}

			morphs.emplace_back(std::move(morph));
		}

		for (auto& morph : morphs)
		{
			std::u16string u16Str = ConvertSjisToU16String(morph.m_morphName.c_str());
			std::string u8Str;
			ConvU16ToU8(u16Str, u8Str);
			morph.m_morphName = u8Str;
		}

		vpd->m_morphs = std::move(morphs);

		return true;
	}

	static std::string ConvertU8ToSjis(const std::string& u8Str)
	{
		std::u16string u16Str;
		if (!ConvU8ToU16(u8Str, u16Str))
		{
			return u8Str;
		}
		return ConvertU16ToSjisString(u16Str);
	}

	bool WriteVPDFile(const VPDFile* vpd, const char* filename)
	{
		File file;
		if (!file.Create(filename))
		{
			LIBMMD_INFO("VPD File Open Fail. {}", filename);
			return false;
		}

		auto writeLine = [&file](const std::string& line) -> bool
		{
			std::string lineWithCRLF = line + "\r\n";
			return file.Write(lineWithCRLF.data(), lineWithCRLF.size());
		};

		writeLine("Vocaloid Pose Data file");
		writeLine("");
		writeLine("miku.osm;\t\t\t\t// parent file name");

		int totalBones = static_cast<int>(vpd->m_bones.size());
		writeLine(std::to_string(totalBones) + ";\t\t\t\t\t\t\t// total bones");
		writeLine("");

		for (int i = 0; i < totalBones; i++)
		{
			const auto& bone = vpd->m_bones[i];
			std::string sjisName = ConvertU8ToSjis(bone.m_boneName);

			writeLine("Bone" + std::to_string(i) + "{" + sjisName);

			char buf[256];
			std::snprintf(buf, sizeof(buf), "  %f,%f,%f;",
				bone.m_translate.x(), bone.m_translate.y(), bone.m_translate.z());
			writeLine(buf);

			std::snprintf(buf, sizeof(buf), "  %f,%f,%f,%f;",
				bone.m_quaternion.x(), bone.m_quaternion.y(),
				bone.m_quaternion.z(), bone.m_quaternion.w());
			writeLine(buf);

			writeLine("}");
			writeLine("");
		}

		int totalMorphs = static_cast<int>(vpd->m_morphs.size());
		for (int i = 0; i < totalMorphs; i++)
		{
			const auto& morph = vpd->m_morphs[i];
			std::string sjisName = ConvertU8ToSjis(morph.m_morphName);

			writeLine("Morph" + std::to_string(i) + "{" + sjisName);

			char buf[64];
			std::snprintf(buf, sizeof(buf), "  %f;", morph.m_weight);
			writeLine(buf);

			writeLine("}");
			writeLine("");
		}

		LIBMMD_INFO("VPD File Write Successed. {}", filename);
		return true;
	}
}