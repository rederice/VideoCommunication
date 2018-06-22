#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#define BLOCK 8
#define _STEP_ 4
int abs(int a)
{
	return a >= 0 ? a : -a;
}
double PSNR(double a, int FRAMESIZE)
{
	return 10*log10(255*255*(double)(FRAMESIZE)/a);
}
double FS(const unsigned char* Y, const unsigned char* Y2, const int FrameRow, const int FrameCol)
{
	int pixel1[FrameRow][FrameCol]={0}, pixel2[FrameRow][FrameCol]={0};
	for(int i=0; i<FrameCol; i++)
		for(int j=0; j<FrameRow; j++) {
			pixel1[j][i] = (int)(Y[FrameRow*i + j]);
			pixel2[j][i] = (int)(Y2[FrameRow*i + j]);
		}
	double psnr = 0.0;
	for(int idx_x=0; idx_x<FrameRow; idx_x += BLOCK) {
		for(int idx_y=0; idx_y<FrameCol; idx_y += BLOCK) {
			// start to run through Y1 macroblocks //
			int minSAD = 1e9;
			int min_x = 0, min_y = 0;
			int STEP = _STEP_;
			for(int srh_x = -STEP+idx_x; srh_x <= STEP+idx_x; srh_x++){
				for(int srh_y = -STEP+idx_y; srh_y <= STEP+idx_y; srh_y++){
					// start to run through Y2 +-4 pixels //
					if(srh_x < 0 || srh_x >= FrameRow || srh_y < 0 || srh_y >= FrameCol) continue;
					int tmpSAD = 0;
					for(int i=0; i<BLOCK; i++)
						for(int j=0; j<BLOCK; j++) // run in a macroblock //
							tmpSAD += abs(pixel1[idx_x + i][idx_y + j] - pixel2[srh_x + i][srh_y + j]);
					if(tmpSAD < minSAD) {
						minSAD = tmpSAD;
						min_x = srh_x;
						min_y = srh_y;
					}
				}
			}
			for(int i=0; i<BLOCK; i++)
				for(int j=0; j<BLOCK; j++)
					psnr += (pixel1[idx_x + i][idx_y + j] - pixel2[min_x + i][min_y + j])*(pixel1[idx_x + i][idx_y + j] - pixel2[min_x + i][min_y + j]);
		}
	}
	return PSNR(psnr, FrameRow*FrameCol);
}
double TSS(const unsigned char* Y, const unsigned char* Y2, const int FrameRow, const int FrameCol)
{
	int pixel1[FrameRow][FrameCol]={0}, pixel2[FrameRow][FrameCol]={0};
	for(int i=0; i<FrameCol; i++)
		for(int j=0; j<FrameRow; j++) {
			pixel1[j][i] = (int)(Y[FrameRow*i + j]);
			pixel2[j][i] = (int)(Y2[FrameRow*i + j]);
		}
	double psnr = 0.0;
	for(int idx_x=0; idx_x<FrameRow; idx_x += BLOCK) {
		for(int idx_y=0; idx_y<FrameCol; idx_y += BLOCK) {
			// start to run through Y1 macroblocks //
			int minSAD = 1e9;
			int min_x = idx_x, min_y = idx_y;
			int STEP = _STEP_;
			while(STEP >= 1) {
				int step_x[9] = {min_x-STEP, min_x-STEP, min_x-STEP, min_x, min_x, min_x, min_x+STEP, min_x+STEP, min_x+STEP};
				int step_y[9] = {min_y-STEP, min_y, min_y+STEP, min_y-STEP, min_y, min_y+STEP, min_y-STEP, min_y, min_y+STEP};
				for(int srh=0; srh<9; srh++){ // start to run in Y2 9 positions //
					if(step_x[srh]<0 || step_x[srh]>=FrameRow || step_y[srh]<0 || step_y[srh]>=FrameCol)
						continue;
					int tmpSAD = 0;
					for(int i=0; i<BLOCK; i++)
							for(int j=0; j<BLOCK; j++) // run in a macroblock //
								tmpSAD += abs(pixel1[idx_x + i][idx_y + j] - pixel2[step_x[srh] + i][step_y[srh] + j]);
					if(tmpSAD < minSAD) {
						minSAD = tmpSAD;
						min_x = step_x[srh];
						min_y = step_y[srh];
					}
				}
				STEP /= 2;
			}
			for(int i=0; i<BLOCK; i++)
				for(int j=0; j<BLOCK; j++)
					psnr += (pixel1[idx_x + i][idx_y + j] - pixel2[min_x + i][min_y + j])*(pixel1[idx_x + i][idx_y + j] - pixel2[min_x + i][min_y + j]);
		}
	}
	return PSNR(psnr, FrameRow*FrameCol);
}
double OS(const unsigned char* Y, const unsigned char* Y2, const int FrameRow, const int FrameCol)
{
	int pixel1[FrameRow][FrameCol]={0}, pixel2[FrameRow][FrameCol]={0};
	for(int i=0; i<FrameCol; i++)
		for(int j=0; j<FrameRow; j++) {
			pixel1[j][i] = (int)(Y[FrameRow*i + j]);
			pixel2[j][i] = (int)(Y2[FrameRow*i + j]);
		}
	double psnr = 0.0;
	for(int idx_x=0; idx_x<FrameRow; idx_x += BLOCK) {
		for(int idx_y=0; idx_y<FrameCol; idx_y += BLOCK) {
			// start to run through Y1 macroblocks //
			int minSAD = 1e9;
			int min_x = idx_x, min_y = idx_y;
			int STEP = _STEP_;
			int direction = 1;
			while(STEP >= 1) {
				int step_x[3] = {0}, step_y[3] = {0};
				if(direction == 1){
					step_x[0] = min_x-STEP;
					step_x[1] = min_x;
					step_x[2] = min_x+STEP;
					for(int l=0;l<3;l++)
						step_y[l] = min_y;
					direction = 0;
				}
				else if(direction == 0){
					for(int l=0;l<3;l++)
						step_x[l] = min_x;
					step_y[0] = min_y-STEP;
					step_y[1] = min_y;
					step_y[2] = min_y+STEP;
					direction = 1;
				}
				for(int srh=0; srh<3; srh++){ // start to run in Y2 3 positions //
					if(step_x[srh]<0 || step_x[srh]>=FrameRow || step_y[srh]<0 || step_y[srh]>=FrameCol)
						continue;
					int tmpSAD = 0;
					for(int i=0; i<BLOCK; i++)
							for(int j=0; j<BLOCK; j++) // run in a macroblock //
								tmpSAD += abs(pixel1[idx_x + i][idx_y + j] - pixel2[step_x[srh] + i][step_y[srh] + j]);
					if(tmpSAD < minSAD) {
						minSAD = tmpSAD;
						min_x = step_x[srh];
						min_y = step_y[srh];
					}
				}
				STEP /= 2;
			}			
			for(int i=0; i<BLOCK; i++)
				for(int j=0; j<BLOCK; j++)
					psnr += (pixel1[idx_x + i][idx_y + j] - pixel2[min_x + i][min_y + j])*(pixel1[idx_x + i][idx_y + j] - pixel2[min_x + i][min_y + j]);
		}
	}
	return PSNR(psnr, FrameRow*FrameCol);
}
double CSA(const unsigned char* Y, const unsigned char* Y2, const int FrameRow, const int FrameCol)
{
	int pixel1[FrameRow][FrameCol]={0}, pixel2[FrameRow][FrameCol]={0};
	for(int i=0; i<FrameCol; i++)
		for(int j=0; j<FrameRow; j++) {
			pixel1[j][i] = (int)(Y[FrameRow*i + j]);
			pixel2[j][i] = (int)(Y2[FrameRow*i + j]);
		}
	double psnr = 0.0;
	for(int idx_x=0; idx_x<FrameRow; idx_x += BLOCK) {
		for(int idx_y=0; idx_y<FrameCol; idx_y += BLOCK) {
			// start to run through Y1 macroblocks //
			int minSAD = 1e9;
			int min_x = idx_x, min_y = idx_y;
			int final_diagonal = 0;
			int STEP = _STEP_;
			while(STEP >= 1) {
				if(STEP == 1 && final_diagonal == 1) {
					int step_x[5] = {min_x-STEP, min_x, min_x, min_x+STEP, min_x};
					int step_y[5] = {min_y, min_y-STEP, min_y+STEP, min_y, min_y};
					for(int srh=0; srh<5; srh++){ // start to run in Y2 5 positions //
						if(step_x[srh]<0 || step_x[srh]>=FrameRow || step_y[srh]<0 || step_y[srh]>=FrameCol)
							continue;
						int tmpSAD = 0;
						for(int i=0; i<BLOCK; i++)
								for(int j=0; j<BLOCK; j++) // run in a macroblock //
									tmpSAD += abs(pixel1[idx_x + i][idx_y + j] - pixel2[step_x[srh] + i][step_y[srh] + j]);
						if(tmpSAD < minSAD) {
							minSAD = tmpSAD;
							min_x = step_x[srh];
							min_y = step_y[srh];
						}
					}
					break;
				}
				else {
					int step_x[5] = {min_x-STEP, min_x-STEP, min_x, min_x+STEP, min_x+STEP};
					int step_y[5] = {min_y-STEP, min_y+STEP, min_y, min_y-STEP, min_y+STEP};
					for(int srh=0; srh<5; srh++){ // start to run in Y2 5 positions //
						if(step_x[srh]<0 || step_x[srh]>=FrameRow || step_y[srh]<0 || step_y[srh]>=FrameCol)
							continue;
						int tmpSAD = 0;
						for(int i=0; i<BLOCK; i++)
								for(int j=0; j<BLOCK; j++) // run in a macroblock //
									tmpSAD += abs(pixel1[idx_x + i][idx_y + j] - pixel2[step_x[srh] + i][step_y[srh] + j]);
						if(tmpSAD < minSAD) {
							minSAD = tmpSAD;
							min_x = step_x[srh];
							min_y = step_y[srh];
							if(srh == 0 || srh == 4) final_diagonal = 1;
						}
					}
					STEP /= 2;
				}
			}
			for(int i=0; i<BLOCK; i++)
				for(int j=0; j<BLOCK; j++)
					psnr += (pixel1[idx_x + i][idx_y + j] - pixel2[min_x + i][min_y + j])*(pixel1[idx_x + i][idx_y + j] - pixel2[min_x + i][min_y + j]);
		}
	}
	return PSNR(psnr, FrameRow*FrameCol);
}
double TLS(const unsigned char* Y, const unsigned char* Y2, const int FrameRow, const int FrameCol)
{
	int pixel1[FrameRow][FrameCol]={0}, pixel2[FrameRow][FrameCol]={0};
	for(int i=0; i<FrameCol; i++)
		for(int j=0; j<FrameRow; j++) {
			pixel1[j][i] = (int)(Y[FrameRow*i + j]);
			pixel2[j][i] = (int)(Y2[FrameRow*i + j]);
		}
	double psnr = 0.0;
	for(int idx_x=0; idx_x<FrameRow; idx_x += BLOCK) {
		for(int idx_y=0; idx_y<FrameCol; idx_y += BLOCK) {
			// start to run through Y1 macroblocks //
			int minSAD = 1e9;
			int min_x = idx_x, min_y = idx_y;
			int STEP = _STEP_;
			while(STEP >= 1) {
				if(STEP == 1) {
					int step_x[9] = {min_x-STEP, min_x-STEP, min_x-STEP, min_x, min_x, min_x, min_x+STEP, min_x+STEP, min_x+STEP};
					int step_y[9] = {min_y-STEP, min_y, min_y+STEP, min_y-STEP, min_y, min_y+STEP, min_y-STEP, min_y, min_y+STEP};
					for(int srh=0; srh<9; srh++){ // start to run in Y2 9 positions //
						if(step_x[srh]<0 || step_x[srh]>=FrameRow || step_y[srh]<0 || step_y[srh]>=FrameCol)
							continue;
						int tmpSAD = 0;
						for(int i=0; i<BLOCK; i++)
								for(int j=0; j<BLOCK; j++) // run in a macroblock //
									tmpSAD += abs(pixel1[idx_x + i][idx_y + j] - pixel2[step_x[srh] + i][step_y[srh] + j]);
						if(tmpSAD < minSAD) {
							minSAD = tmpSAD;
							min_x = step_x[srh];
							min_y = step_y[srh];
						}
					}
					break;
				}
				else {
					int step_x[5] = {min_x-STEP, min_x, min_x, min_x, min_x+STEP};
					int step_y[5] = {min_y, min_y+STEP, min_y-STEP, min_y, min_y};
					for(int srh=0; srh<5; srh++){ // start to run in Y2 5 positions //
						if(step_x[srh]<0 || step_x[srh]>=FrameRow || step_y[srh]<0 || step_y[srh]>=FrameCol)
							continue;
						int tmpSAD = 0;
						for(int i=0; i<BLOCK; i++)
								for(int j=0; j<BLOCK; j++) // run in a macroblock //
									tmpSAD += abs(pixel1[idx_x + i][idx_y + j] - pixel2[step_x[srh] + i][step_y[srh] + j]);
						if(tmpSAD < minSAD) {
							minSAD = tmpSAD;
							min_x = step_x[srh];
							min_y = step_y[srh];
						}
					}
					STEP /= 2;
				}
			}
			for(int i=0; i<BLOCK; i++)
				for(int j=0; j<BLOCK; j++)
					psnr += (pixel1[idx_x + i][idx_y + j] - pixel2[min_x + i][min_y + j])*(pixel1[idx_x + i][idx_y + j] - pixel2[min_x + i][min_y + j]);
		}
	}
	return PSNR(psnr, FrameRow*FrameCol);
}
int main(int argc, char const *argv[])
{
	char name[4][16]={"Akiyo.QCIF","Table.QCIF","Foreman.CIF","Stefan.CIF"};
	printf("Frame,FS,TSS,OS,CSA,TLS\n");
	for(int iter=0; iter<4; iter++){
		int count = 0;
		FILE *fp;
		fp = fopen(name[iter], "rb");
		if(fp==NULL) printf("Error opening video %s", name[iter]);
		else {
			if(iter == 0 || iter == 1) {
				int FrameRow = 176;
				int FrameCol = 144;
				int FrameSize = 176*144;
				unsigned char Y[FrameSize], Cb[FrameSize/4], Cr[FrameSize/4];
				unsigned char Y2[FrameSize], Cb2[FrameSize/4], Cr2[FrameSize/4];
				if(fread(Y, FrameSize, 1, fp))
					if(fread(Cb, FrameSize/4, 1, fp))
						if(fread(Cr, FrameSize/4, 1, fp))
							count+=1;
				while(fread(Y2, FrameSize, 1, fp)){
					if(fread(Cb2, FrameSize/4, 1, fp)){
						if(fread(Cr2, FrameSize/4, 1, fp)){
							count+=1;
							printf("%d,", count);
							printf("%lf,", FS(Y2, Y, FrameRow, FrameCol));
							printf("%lf,", TSS(Y2, Y, FrameRow, FrameCol));
							printf("%lf,", OS(Y2, Y, FrameRow, FrameCol));
							printf("%lf,", CSA(Y2, Y, FrameRow, FrameCol));
							printf("%lf\n", TLS(Y2, Y, FrameRow, FrameCol));
						}
					}
					memcpy(Y, Y2, sizeof(Y2));
				}
			}
			else if(iter == 2 || iter == 3) {
				int FrameRow = 352;
				int FrameCol = 288;
				int FrameSize = 352*288;
				unsigned char Y[FrameSize], Cb[FrameSize/4], Cr[FrameSize/4];
				unsigned char Y2[FrameSize], Cb2[FrameSize/4], Cr2[FrameSize/4];
				if(fread(Y, FrameSize, 1, fp))
					if(fread(Cb, FrameSize/4, 1, fp))
						if(fread(Cr, FrameSize/4, 1, fp))
							count+=1;
				while(fread(Y2, FrameSize, 1, fp)){
					if(fread(Cb2, FrameSize/4, 1, fp)){
						if(fread(Cr2, FrameSize/4, 1, fp)){
							count+=1;
							printf("%d,", count);
							printf("%lf,", FS(Y2, Y, FrameRow, FrameCol));
							printf("%lf,", TSS(Y2, Y, FrameRow, FrameCol));
							printf("%lf,", OS(Y2, Y, FrameRow, FrameCol));
							printf("%lf,", CSA(Y2, Y, FrameRow, FrameCol));
							printf("%lf\n", TLS(Y2, Y, FrameRow, FrameCol));
						}
					}
					memcpy(Y, Y2, sizeof(Y2));
				}
			}
		}
	}
	return 0;
}