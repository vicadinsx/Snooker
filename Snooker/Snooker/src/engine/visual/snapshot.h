#pragma once
#ifndef __Snapshot_H__
#define __Snapshot_H__

#include <string>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include <Windows.h>
#include <windef.h>

namespace engine {

	class Snapshot {
		int snaps_taken = 0;

		Snapshot() {}
		~Snapshot() {}
		Snapshot(Snapshot const&);		// Don't Implement.
		void operator=(Snapshot const&);		// Don't implement

	public:
		static Snapshot* instance()
		{
			static Snapshot instance;
			return &instance;
		}

		bool takeSnapshotToTGA(int w, int h) {
			std::string filename = "data/snapshots/snapshot_" + std::to_string(snaps_taken++) + ".tga";
			//This prevents the images getting padded 
			// when the width multiplied by 3 is not a multiple of 4
			glPixelStorei(GL_PACK_ALIGNMENT, 1);

			int nSize = w*h * 3;
			// First let's create our buffer, 3 channels per Pixel
			char* dataBuffer = (char*)malloc(nSize * sizeof(char));

			if (!dataBuffer) return false;
				
			// Let's fetch them from the backbuffer	
			// We request the pixels in GL_BGR format, thanks to Berzeger for the tip
			glReadBuffer(GL_FRONT);
			glReadPixels((GLint)0, (GLint)0,
				(GLint)w, (GLint)h,
				GL_BGR, GL_UNSIGNED_BYTE, dataBuffer);

			//Now the file creation
			//FILE *filePtr = fopen(filename.c_str(), "wb");
			FILE *filePtr;
			fopen_s(&filePtr, filename.c_str(), "wb");
			if (!filePtr) return false;

			unsigned char TGAheader[12] = { 0,0,2,0,0,0,0,0,0,0,0,0 };
			unsigned char header[6] = { w % 256,w / 256,
				h % 256,h / 256,
				24,0 };
			// We write the headers
			fwrite(TGAheader, sizeof(unsigned char), 12, filePtr);
			fwrite(header, sizeof(unsigned char), 6, filePtr);
			// And finally our image data
			fwrite(dataBuffer, sizeof(GLubyte), nSize, filePtr);
			fclose(filePtr);

			free(dataBuffer);

			return true;
		}

		bool takeSnapshotToBMP(int w, int h) {
			//std::string filename = "data/snapshots/snapshot_" + std::to_string(snaps_taken++) + ".bmp";

			//BYTE* bmpBuffer = (BYTE*)malloc(w*h * 3);
			//if (!bmpBuffer)
			//	return false;

			/*glReadBuffer(GL_FRONT);
			glReadPixels((GLint)0, (GLint)0,
				(GLint)w - 1, (GLint)h - 1,
				GL_RGB, GL_UNSIGNED_BYTE, bmpBuffer);*/

			/*//FILE *filePtr = fopen(filename.c_str(), "wb");
			FILE *filePtr;
			fopen_s(&filePtr, filename.c_str(), "wb");
			if (!filePtr)
				return false;*/

			//BITMAPFILEHEADER bitmapFileHeader;
			//BITMAPINFOHEADER bitmapInfoHeader;

			//bitmapFileHeader.bfType = 0x4D42; //"BM"
			//bitmapFileHeader.bfSize = w*h * 3;
			//bitmapFileHeader.bfReserved1 = 0;
			//bitmapFileHeader.bfReserved2 = 0;
			//bitmapFileHeader.bfOffBits =
			//	sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

			//bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
			//bitmapInfoHeader.biWidth = w - 1;
			//bitmapInfoHeader.biHeight = h - 1;
			//bitmapInfoHeader.biPlanes = 1;
			//bitmapInfoHeader.biBitCount = 24;
			//bitmapInfoHeader.biCompression = BI_RGB;
			//bitmapInfoHeader.biSizeImage = 0;
			//bitmapInfoHeader.biXPelsPerMeter = 0; // ?
			//bitmapInfoHeader.biYPelsPerMeter = 0; // ?
			//bitmapInfoHeader.biClrUsed = 0;
			//bitmapInfoHeader.biClrImportant = 0;

			/*fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
			fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
			fwrite(bmpBuffer, w*h * 3, 1, filePtr);
			fclose(filePtr);*/

			/*free(bmpBuffer);

			return true;*/

			//SECOND TRY - BOTH BLUE WALL
			std::string filename = "data/snapshots/snapshot_" + std::to_string(snaps_taken++) + ".bmp";
			int filesize = 54 + 3 * w*h;

			char* bmpBuffer = (char*)malloc(3 * w*h * sizeof(char));
			glReadBuffer(GL_FRONT);
			glReadPixels((GLint)0, (GLint)0,
				(GLint)w - 1, (GLint)h - 1,
				GL_BGR, GL_UNSIGNED_BYTE, bmpBuffer);

			//FILE *filePtr = fopen(filename.c_str(), "wb");
			FILE *filePtr;
			fopen_s(&filePtr, filename.c_str(), "wb");
			if (!filePtr)
				return false;

			unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
			unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };

			bmpfileheader[2] = (unsigned char)(filesize);
			bmpfileheader[3] = (unsigned char)(filesize >> 8);
			bmpfileheader[4] = (unsigned char)(filesize >> 16);
			bmpfileheader[5] = (unsigned char)(filesize >> 24);

			bmpinfoheader[4] = (unsigned char)(w);
			bmpinfoheader[5] = (unsigned char)(w >> 8);
			bmpinfoheader[6] = (unsigned char)(w >> 16);
			bmpinfoheader[7] = (unsigned char)(w >> 24);
			bmpinfoheader[8] = (unsigned char)(h);
			bmpinfoheader[9] = (unsigned char)(h >> 8);
			bmpinfoheader[10] = (unsigned char)(h >> 16);
			bmpinfoheader[11] = (unsigned char)(h >> 24);

			fwrite(bmpfileheader, 1, 14, filePtr);
			fwrite(bmpinfoheader, 1, 40, filePtr);
			fwrite(bmpBuffer, w*h * 3, 1, filePtr);
			fclose(filePtr);

			free(bmpBuffer);

			return true;
		}

	};
}

#endif
