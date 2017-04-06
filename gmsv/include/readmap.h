#ifndef __MAP_H__
#define __MAP_H__
#include "common.h"
#include "util.h"

typedef struct tagMAP_Objlink
{
    int objindex;
    struct tagMAP_Objlink* next;
}MAP_Objlink, *OBJECT;

#define GET_OBJINDEX(x) ((x)->objindex)
#define NEXT_OBJECT(x) ((x)->next)

typedef struct tagMAP_Map
{
    int     id;             /*        ID    */
    int     xsiz,ysiz;      /*          */
    char    string[64];     /*  ��ʾ    */
    unsigned short*  tile;           /*          */
    unsigned short*  obj;            /*                  */
    MAP_Objlink** olink;
#ifdef _MAP_NOEXIT
	unsigned int startpoint;
	int MapType;
#endif
}MAP_Map;


typedef enum
{
    MAP_WALKABLE,           /*
                             *         ����
                             * ��? Q            ��     0
                             * ��? M        ��   1
                             * ��? M        ��     2
                             */
    MAP_HAVEHEIGHT,         /*  ��            ��0           0   */
    MAP_DEFENCE,            /*  �\        ���ر�����0 ����    �\        */

    MAP_INTODAMAGE,         /*        ��    ʱ  HP  ��  ?  */
    MAP_OUTOFDAMAGE,        /*          ��  ʱ  HP  ��  ?  */

    MAP_SETED_BATTLEMAP,    /*             �趨? */
	MAP_BATTLEMAP,			/*               ���A ��*/
	MAP_BATTLEMAP2,			/*               ���A ��*/
	MAP_BATTLEMAP3,			/*               ���A ��*/

/* ����  �̣ӣ�  ʹ        �ӣ�    ʹ      (? X      ? @) */

    MAP_INTODARKNESS,       /*  ��    ʱ  ��      */
    MAP_INTOCONFUSION,      /*  ��    ʱ  ��?    */

    MAP_OUTOFPOISON,         /*  ��    ʱ  ��    */
    MAP_OUTOFPARALYSIS,      /*  ��    ʱ            */
    MAP_OUTOFSILENCE,        /*  ��    ʱ  ��?  */
    MAP_OUTOFSTONE,          /*  ��    ʱ  ʯ    */
    MAP_OUTOFDARKNESS,       /*  ��    ʱ  ��      */
    MAP_OUTOFCONFUSION,      /*  ��    ʱ  ��?    */

    MAP_DATAINT_NUM,
}MAP_DATAINT;
typedef enum
{
    MAP_DATACHAR_NUM,
}MAP_DATACHAR;
typedef struct tagMAP_ImageData
{
    int     data[MAP_DATAINT_NUM];
    STRING32    string[MAP_DATACHAR_NUM];
}MAP_ImageData;

typedef enum
{
    MAP_KINDWALKABLE,           /*  ��? F   1   ��     */

    MAP_KINDNUM,
}MAP_kind;


INLINE int MAP_getfloorIndex( int floorid );
int MAP_getfloorX( int floorid );
int MAP_getfloorY( int floorid );

BOOL MAP_initReadMap( char* maptilefile , char* mapdir );
BOOL MAP_initMapArray( int   num );
void    MAP_endMapArray( void );

char* MAP_getdataFromRECT( int floor, RECT* seekr, RECT* realr );
char *MAP_getChecksumFromRECT( int floor, RECT* seekr, RECT* realr,
								int *tilesum, int *objsum, int *eventsum );

BOOL MAP_checkCoordinates( int mapid, int x, int y );

BOOL MAP_setTileAndObjData( int ff ,int fx, int fy, int tile, int obj);
BOOL MAP_getTileAndObjData( int ff ,int fx, int fy, int* tile, int* obj);
void MAP_sendAroundMapdata( int fl, int x, int y);
int MAP_getImageInt( int imagenumber, int element );
BOOL MAP_setImageInt( int imagenumber, int element, int value );
BOOL IsValidImagenumber( int imagenumber );

char* MAP_getfloorShowstring( int floorid );
BOOL MAP_makeVariousMap(char* atile, char* aobj, int floor, int startx, int starty, int xsiz, int ysiz, MAP_kind   kind );
BOOL MAP_makeWalkableMap( char* data,  int floor, int startx, int starty,int xsiz, int ysiz );
BOOL MAP_IsThereSpecificFloorid( int floorid );
BOOL MAP_IsValidCoordinate( int floorid, int x, int y );

BOOL MAP_addNewObj( int floor, int x, int y, int objindex );
BOOL MAP_removeObj( int floor, int x, int y, int objindex );
#define		MAP_getTopObj( fl, x, y)	_MAP_getTopObj( __FILE__, __LINE__, fl, x, y)
MAP_Objlink* _MAP_getTopObj( char *file, int line, int floor, int x, int y );
#define		MAP_objmove( objindex, of, ox, oy, nfl, nx, ny) _MAP_objmove( __FILE__, __LINE__, objindex, of, ox, oy, nfl, nx, ny)
BOOL _MAP_objmove( char *file, int line, int objindex, int ofloor, int ox, int oy, int nfloor,
                  int nx, int ny );
char *MAP_getFloorName( int floor);
BOOL MAP_setObjData( int ff ,int fx, int fy, int obj, int objhp );

#ifdef _MAKE_MAP
int MAP_getFloorXY( int floor, int *x, int *y);
#endif

#ifdef _MAP_NOEXIT
unsigned int MAP_getExFloor_XY( int floor , int *map_type);
BOOL CHECKFLOORID( int id);
#endif

#endif 
/*__MAP_H__*/
