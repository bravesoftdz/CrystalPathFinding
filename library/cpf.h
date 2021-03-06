/********************************************************************************/
/* Copyright (c) 2011-2015 Dmitry Mozulyov                                      */
/*                                                                              */
/* Permission is hereby granted, free of charge, to any person obtaining a copy */
/* of this software and associated documentation files (the "Software"), to deal*/
/* in the Software without restriction, including without limitation the rights */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell    */
/* copies of the Software, and to permit persons to whom the Software is        */
/* furnished to do so, subject to the following conditions:                     */
/*                                                                              */
/* The above copyright notice and this permission notice shall be included in   */
/* all copies or substantial portions of the Software.                          */
/*                                                                              */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,*/
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN    */
/* THE SOFTWARE.                                                                */
/*                                                                              */
/* email: softforyou@inbox.ru                                                   */
/* skype: dimandevil                                                            */
/* repository: https://github.com/d-mozulyov/CrystalPathFinding                 */
/********************************************************************************/

#ifndef CRYSTAL_PATH_FINDING_H
#define CRYSTAL_PATH_FINDING_H
ToDo: object oriented interface implementation & Memory/Exception callbacks (cpfInitialize)

  typedef unsigned short ushort;
  typedef unsigned char uchar;
  struct TPoint
  {
      long X;
      long Y;
      TPoint(){}
      TPoint(long x, long y):X(x),Y(y){}
  };  
 
  // handle type
  typedef size_t TCPFHandle;
     
  // kind of map
  typedef uchar TTileMapKind; enum {mkSimple, mkDiagonal, mkDiagonalEx, mkHexagonal}; 

  // result of find path function
  struct TTileMapPath
  {
      size_t  Index;  
	  TPoint* Points;
      size_t  Count;
      double  Distance;
  };

  // path finding parameters
  struct TTileMapParams
  {
      TPoint* Starts;
      size_t StartsCount;
      TPoint Finish;
      TCPFHandle Weights;
      TPoint* Excludes;
      size_t ExcludesCount;
  };

  // map tile barrier
  #define TILE_BARRIER 0  

  // object oriented Weights interface
  struct TTileMapWeights
  {
	// constructor and destructor
	TTileMapWeights();
	~TTileMapWeights();

    // internal descriptor
    TCPFHandle Handle; 

    // tile weight values
	float getValue(uchar Tile);
	void setValue(uchar Tile, float Value);
  }; 

  // object oriented Map interface
  struct TTileMap
  {
	// constructor and destructor
	TTileMap(ushort AWidth, ushort AHeight, TTileMapKind AKind, bool ASameDiagonalWeight = false);
	~TTileMap();

    // internal descriptor
    TCPFHandle Handle; 

    // basic parameters
    ushort Width;
	ushort Height;
	TTileMapKind Kind;
	bool SameDiagonalWeight;

    // important variable parameters!
	bool SectorTest;
	bool Caching;

	// update methods
	void Clear();
	void Update(uchar* ATiles, ushort X, ushort Y, ushort AWidth, ushort AHeight, ptrdiff_t Pitch = 0);
	uchar getTile(ushort X, ushort Y);
	void setTile(ushort X, ushort Y, uchar Value);

	// path finding
	TTileMapPath FindPath(const TTileMapParams Params, bool FullPath = true);
	TTileMapPath FindPath(const TPoint Start, const TPoint Finish,
	  TCPFHandle Weights = 0, TPoint* Excludes = NULL, size_t ExcludesCount = 0, bool FullPath = true);
	TTileMapPath FindPath(TPoint* Starts, size_t StartsCount, const TPoint Finish,
	  TCPFHandle Weights = 0, TPoint* Excludes = NULL, size_t ExcludesCount = 0, bool FullPath = true);
  }; 
  
  

//  initialization/finalization  routine
namespace cpf_routine
{
  HINSTANCE cpf_dll=0;

  typedef TCPFHandle (*CPF_PROC_CREATE_WEIGHTS)();
  typedef void (*CPF_PROC_DESTROY_WEIGHTS)(TCPFHandle& HWeights);
  typedef float (*CPF_PROC_WEIGHT_GET)(TCPFHandle HWeights, byte Tile);
  typedef void (*CPF_PROC_WEIGHT_SET)(TCPFHandle HWeights, byte Tile, float Value);
  typedef void (*CPF_PROC_WEIGHTS_CLEAR)(TCPFHandle HWeights);
  typedef TCPFHandle (*CPF_PROC_CREATE_MAP)(word Width, word Height, TTileMapKind Kind);
  typedef void (*CPF_PROC_DESTROY_MAP)(TCPFHandle& HMap);
  typedef void (*CPF_PROC_MAP_CLEAR)(TCPFHandle HMap, bool SameDiagonalWeight);
  typedef byte (*CPF_PROC_MAP_GET_TILE)(TCPFHandle HMap, word X, word Y);
  typedef void (*CPF_PROC_MAP_SET_TILE)(TCPFHandle HMap, word X, word Y, byte Value);
  typedef void (*CPF_PROC_MAP_UPDATE)(TCPFHandle HMap, byte* Tiles, word X, word Y, word Width, word Height, size_i Pitch);
  typedef TTileMapPath (*CPF_PROC_FIND_PATH)(TCPFHandle HMap, TTileMapParams* Params, bool SectorTest, bool Caching, bool FullPath);

  CPF_PROC_CREATE_WEIGHTS __cpfCreateWeights = NULL;
  CPF_PROC_DESTROY_WEIGHTS __cpfDestroyWeights = NULL;
  CPF_PROC_WEIGHT_GET __cpfWeightGet = NULL;
  CPF_PROC_WEIGHT_SET __cpfWeightSet = NULL;
  CPF_PROC_WEIGHTS_CLEAR __cpfWeightsClear = NULL;
  CPF_PROC_CREATE_MAP __cpfCreateMap = NULL;
  CPF_PROC_DESTROY_MAP __cpfDestroyMap = NULL;
  CPF_PROC_MAP_CLEAR __cpfMapClear = NULL;
  CPF_PROC_MAP_GET_TILE __cpfMapGetTile = NULL;
  CPF_PROC_MAP_SET_TILE __cpfMapSetTile = NULL;
  CPF_PROC_MAP_UPDATE __cpfMapUpdate = NULL;
  CPF_PROC_FIND_PATH __cpfFindPath = NULL;


struct TCpfInitializator
{
  TCpfInitializator()
  {
    if (cpf_dll) return;
    cpf_dll = LoadLibraryA("cpf.dll");
    if (!cpf_dll) return;

    __cpfCreateWeights = (CPF_PROC_CREATE_WEIGHTS)GetProcAddress(cpf_dll, "cpfCreateWeights");
    __cpfDestroyWeights = (CPF_PROC_DESTROY_WEIGHTS)GetProcAddress(cpf_dll, "cpfDestroyWeights");
    __cpfWeightGet = (CPF_PROC_WEIGHT_GET)GetProcAddress(cpf_dll, "cpfWeightGet");
    __cpfWeightSet = (CPF_PROC_WEIGHT_SET)GetProcAddress(cpf_dll, "cpfWeightSet");
	__cpfWeightsClear = (CPF_PROC_WEIGHTS_CLEAR)GetProcAddress(cpf_dll, "cpfWeightsClear");
    __cpfCreateMap = (CPF_PROC_CREATE_MAP)GetProcAddress(cpf_dll, "cpfCreateMap");
    __cpfDestroyMap = (CPF_PROC_DESTROY_MAP)GetProcAddress(cpf_dll, "cpfDestroyMap");
    __cpfMapClear = (CPF_PROC_MAP_CLEAR)GetProcAddress(cpf_dll, "cpfMapClear");
    __cpfMapUpdate = (CPF_PROC_MAP_UPDATE)GetProcAddress(cpf_dll, "cpfMapUpdate");
    __cpfMapGetTile = (CPF_PROC_MAP_GET_TILE)GetProcAddress(cpf_dll, "cpfMapGetTile");
    __cpfMapSetTile = (CPF_PROC_MAP_SET_TILE)GetProcAddress(cpf_dll, "cpfMapSetTile");
    __cpfFindPath = (CPF_PROC_FIND_PATH)GetProcAddress(cpf_dll, "cpfFindPath");
  }

  ~TCpfInitializator()
  {
     if (cpf_dll)
     {
       FreeLibrary(cpf_dll);
       cpf_dll = 0;
     }
  }
};

  TCpfInitializator CpfInitializator;
};




//  -------------------------  used functions  --------------------------------
TCPFHandle cpfCreateWeights()/*;*/{return cpf_routine::__cpfCreateWeights();}
void    cpfDestroyWeights(TCPFHandle& HWeights)/*;*/{cpf_routine::__cpfDestroyWeights(HWeights);}
float   cpfWeightGet(TCPFHandle HWeights, byte Tile)/*;*/{return cpf_routine::__cpfWeightGet(HWeights, Tile);}
void    cpfWeightSet(TCPFHandle HWeights, byte Tile, float Value)/*;*/{cpf_routine::__cpfWeightSet(HWeights, Tile, Value);}
void    cpfWeightsClear(TCPFHandle HWeights)/*;*/{cpf_routine::__cpfWeightsClear(HWeights);}
TCPFHandle cpfCreateMap(word Width, word Height, TTileMapKind Kind, bool SameDiagonalWeight = false)/*;*/{return cpf_routine::__cpfCreateMap(Width, Height, Kind, SameDiagonalWeight);}
void    cpfDestroyMap(TCPFHandle& HMap)/*;*/{cpf_routine::__cpfDestroyMap(HMap);}
void    cpfMapClear(TCPFHandle HMap)/*;*/{cpf_routine::__cpfMapClear(HMap);}
byte cpfMapGetTile(TCPFHandle HMap, word X, word Y)/*;*/{return cpf_routine::__cpfMapGetTile(HMap, X, Y);}
void    cpfMapSetTile(TCPFHandle HMap, word X, word Y, byte Value)/*;*/{cpf_routine::__cpfMapSetTile(HMap, X, Y, Value);};
void    cpfMapUpdate(TCPFHandle HMap, byte* Tiles, word X, word Y, word Width, word Height, size_i Pitch=0)/*;*/{cpf_routine::__cpfMapUpdate(HMap, Tiles, X, Y, Width, Height, Pitch);}
TTileMapPath cpfFindPath(TCPFHandle HMap, TTileMapParams* Params, bool SectorTest=true, bool Caching=true, bool FullPath=true)/*;*/{return cpf_routine::__cpfFindPath(HMap, Params, SectorTest, Caching, FullPath);}



#endif
