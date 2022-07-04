#include "tarutilities.h"
#include <fstream>
#include "tarlib.h"

namespace tarlib
{
   namespace utils
   {
		bool isValidTarFile(std::wstring const &filename)
		{
			std::fstream tarfile;

			tarfile.open(filename.c_str(), std::ios_base::in | std::ios_base::binary);

			if (tarfile.fail())
			{
				return false;
			}

			tarHeaderAscii header;

			memset((char*)&header, 0, sizeof(header));

			tarfile.read((char*)&header, sizeof(header));

			const char* expectedUstar = "ustar";

			if (strncmp(header.header.magicwithversion, expectedUstar, 5) != 0)
			{
				return false;
			}

			auto readChecksum = tarHeader::fromAscii(header).checksum;
			auto calculatedChecksum = tarHeader::get_checksum(header);

			return calculatedChecksum == readChecksum;
		}
   }
}