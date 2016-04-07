/*************************************
 *网络通讯 数据结构
 *date:     2014.10.22
 *changelog:
 *
 *************************************/

#ifndef _HY_PROTOCL_H
#define _HY_PROTOCL_H

//信令通道
//下发的时候：先发头，再发内容
//接收的时候：只接收头
//1.	心跳包，len = 0  山火&外破
#define ATOD_KEEP_HEART_BEAT		0x61646301
#define DTOA_KEEP_HEART_BEAT		0x64616301
//2.	实时流，len = 8  山火&外破
#define ATOD_REAL_TIME_REQUEST		0x61646302
#define DTOA_REAL_TIME_RSP			0x64616302
//3.	ptz,，len = 12  山火&外破
#define ATOD_SYS_PTZ					0x61646303
#define DTOA_SYS_PTZ					0x64616303
//4.	升级,，len = 0
#define ATOD_SYS_UPDATE				0x61646304//山火&外破处理平台升级
#define DTOA_SYS_UPDATE				0x64616304//山火&外破处理平台升级
#define ATOD_MK60_1_UPDATE			0x61646305//MK60 1 升级 和A9直接连接的MK60
#define DTOA_MK60_1_UPDATE			0x64616305//MK60 1 升级
#define ATOD_MK60_2_UPDATE			0x61646311//MK60 2 升级
#define DTOA_MK60_2_UPDATE			0x64616311//MK60 2 升级
#define ATOD_MK60_EMER_UPDATE	    0x61646314//mk60应急升级
#define DTOA_MK60_EMER_UPDATE		0x64616314
#define ATOD_PTZ_MANU				0x61646306//ptz手动指令,山火&外破
#define DTOA_PTZ_MANU				0x64616306
#define ATOD_PTZ_AUTO				0x61646307//ptz自动指令,山火&外破
#define DTOA_PTZ_AUTO				0x64616307
//5.	A9重启(这边不需要做)
#define	ATOD_A9_RESTART				0x61646308//A9Reboot,山火&外破
#define	DTOA_A9_RESTART				0x64616308//A9Reboot,山火&外破
//6.	设置扫描范围
#define ATOD_PTZ_RANGE		0x61646309
#define	DTOA_PTZ_RANGE		0x64616309
//7.	火源屏蔽
#define ATOD_CONFIG_NORMAL_FIRE		0x61646310
#define DTOA_CONFIG_NORMAL_FIRE		0x64616310
//8.	下发系统时间
#define ATOD_SET_ARM_REAL_TIME		0x61646312//山火&外破
#define DTOA_SET_ARM_REAL_TIME		0x64616312//山火&外破
//9.	管理预置点
#define ATOD_MANAGE_PRESET			0x61646313//山火&外破
#define DTOA_MANAGE_PRESET			0x64616313//山火&外破
//10.	设置红外探测告警阀值
#define ATOD_SET_AlARM_PARAM		0x61646315
#define DTOA_SET_ALARM_PARAM		0x64616315
//11.	3G流量统计
#define ATOD_MOBILE_FLOW_VALUE		0x61646316//获取DVR统计的3g流量,山火&外破
#define DTOA_MOBILE_FLOW_VALUE		0x64616316//山火&外破
//12.	开启单次自动聚焦
#define ATOD_SET_IR_FOCUS			0x61646317
#define DTOA_SET_IR_FOCUS			0x64616317
#define ATOD_SET_IR_FOCUS_ADD		0x61646318//红外相机手动聚焦+
#define DTOA_SET_IR_FOCUS_ADD		0x64616318
#define ATOD_SET_IR_FOCUS_SUB		0x61646319//红外相机手动聚焦-
#define DTOA_SET_IR_FOCUS_SUB		0x64616319
#define	ATOD_GET_IR_PHOTO			0x61646320//获取红外相机图片
#define	DTOA_GET_IR_PHOTO			0x64616320
#define	ATOD_IR_FOCUS_ADJUST		0x61646321//获取红外相机图片
#define	DTOA_IR_FOCUS_ADJUST		0x64616321
#define	ATOD_SET_DETECT_TIME		0x61646322//设置探测时间
#define	DTOA_SET_DETECT_TIME		0x64616322
#define	ATOD_SET_VIDEO_CONFIG		0x61646323//设置视频编码参数
#define	DTOA_SET_VIDEO_CONFIG		0x64616323
//13.	可见光调焦预置位（查看回放时使用）
#define	ATOD_SET_ZOOM_PRESET		0x61646324
#define	DTOA_SET_ZOOM_PRESET		0x64616324
//14.	获取可见光相机图片
#define	ATOD_GET_VISU_PHOTO			0x61646325//获取可见光相机图片
#define	DTOA_GET_VISU_PHOTO			0x64616325
//15.	设置红外探测面积告警阀值
#define ATOD_SET_AlARM_AREA_PARAM	0x61646326
#define DTOA_SET_ALARM_AREA_PARAM	0x64616326
//16.	设置场景类型
#define ATOD_SET_SCENE_TYPE			0x61646327
#define DTOA_SET_SCENE_TYPE			0x64616327
//17.	获取场景
#define ATOD_GET_SCENE				0x61646328
#define DTOA_GET_SCENE				0x64616328
//18.	ping可见光摄像机
#define ATOD_PING_CAMERA			0x61646329//山火&外破
#define DTOA_PING_CAMERA			0x64616329//山火&外破
//19.	山火等的巡航路径
#define	ATOD_PTZ_TOUR				0x61646331//onvif预置路径操作
#define	DTOA_PTZ_TOUR				0x64616331
//20.    计划任务
#define	ATOD_VIDEO_RECORD_TASK		0x61646332//计划录像任务
#define	DTOA_VIDEO_RECORD_TASK		0x64616332
#define	ATOD_PICTURE_RECORD_TASK	0x61646333//计划截图任务
#define	DTOA_PICTURE_RECORD_TASK	0x64616333
//22.  同步海康相机时间
#define	ATOD_SYNC_CAMERA_TIME		0x61646335
#define	DTOA_SYNC_CAMERA_TIME		0x64616335

//21.    下面四个都是文件传输，registrat相机视场校准
#define	ATOD_REGISTRATION_PARAMS	0x61646334//图片重合参数
#define	DTOA_REGISTRATION_PARAMS	0x64616334
//23.  阈值文件下载
#define	 ATOD_THRESHOTLD_FILE_DOWNLOAD 0x61646336
#define	 DTOA_THRESHOTLD_FILE_DOWNLOAD 0x64616336
//24.  地理信息数据库下载
#define	 ATOD_GEOPOS_FILE_DOWNLOAD	 0x61646337
#define	 DTOA_GEOPOS_FILE_DOWNLOAD	 0x64616337
//25.  共享库升级
#define	 ATOD_LINUX_LIB_DOWNLOAD	 0x61646338
#define	 DTOA_LINUX_LIB_DOWNLOAD	 0x64616338
//26.  关闭实时流，新增 2015-8-14
#define 	ATOD_REAL_STREAM_STOP	0x61646339//
#define 	DTOA_REAL_STREAM_STOP	0x64616339
//27.  设置外破侦测区域，新增2015-8-14 Jimmy
#define 	ATOD_SET_DETECT_AREA	0x61646343
#define 	DTOA_SET_DETECT_AREA	0x64616343
//28.  获取外破侦测区域,新增2015-8-24 Jimmy
#define     ATOD_GET_DETECT_AREA    0x61646344
#define 	DTOA_GET_DETECT_AREA	0x64616344

//数据通道
//1．	图片,山火&外破
#define DTOA_PICTURE_INFO				0x64616901
//2．	录像,山火&外破
#define DTOA_RECORD_FILE_INFO			0x64616902
//3．	实时流
#define DTOA_REAL_TIME_STREAM_INFO		0x64616903
//4．	报警信息,山火&外破
#define DTOA_ALARM_INFO					0x64616904
//5.	升级信息
#define DTOA_MK60_1_UPDATE_INFO		    0x64616905//mk60升级信息，但是处理平台的升级信息在每次重启的时候由信令通道发送给接入服务器
//6.	扫描范围设置响应
#define DTOA_PTZ_RANGE_INFO		        0x64616906//mk60 ptz range信息
//7.	可见光图片
#define DTOA_VISU_PICTURE_INFO			0x64616907//
//8.	设备状态,山火&外破
#define DTOA_DEVICE_STATUS_INFO			0x64616908
//9.	Dvr日志
#define DTOA_LOG_INFO					0x64616909
//10.	3g流量,山火&外破
#define DTOA_MOBILE_FLOW_INFO     		0x6461690A
//11.	3G模块状态,山火&外破
#define DTOA_3G_MODULE_STATUS     		0x6461690B
//12.	Dvr数据库
#define DTOA_DB_INFO					0x6461690C
//13.	Dvr场景图片
#define DTOA_SCENE_INFO			        0x6461690E
//14.	Dvr可见光场景图片
#define DTOA_VISU_SCENE_INFO			0x6461690F
//15.    计划任务
#define DTOA_VEDIO_TASK_INFO		0x64616911//计划录像
#define	DTOA_PICTURE_TASK_INFO		0x64616912//计划截图

//PTZ指令
#define MOVE_UP 0x0402
#define MOVE_L_UP 0x0702
#define MOVE_R_UP 0x0802
#define MOVE_DOWN 0x0404
#define MOVE_L_DOWN 0x0704
#define MOVE_R_DOWN 0x0804
#define MOVE_L 0x0504
#define MOVE_R 0x0502
#define MOVE_STOP 0x0901
#define VISU_CAM_SWITH	    	0x0902 //可见光摄像机开关
#define IR_CAM_SWITH	    	0x0903 //红外摄像机开关
#define A9_POWER_OFF			0x0904 //A9断电
#define VISU_CAM_ZOOM			0x0905 //可见光变倍
#define VISU_CAM_ZOOM_STOP      0x0906 //可见变倍停止
#define VISU_CAM_FOCUS			0x0907 //可见光相机聚焦 0x01聚焦近 0x02聚焦远
#define VISU_CAM_FOCUS_STOP		0x0906 //可见光相机聚焦 停止
#define ZOOM_IN 0x0302//外破
#define ZOOM_OUT 0x0304//外破
#define ZOOM_IN_STOP 0x0305
#define ZOOM_OUT_STOP 0x0306
#define FOCUS_IN 0x0202
#define FOCUS_OUT 0x0204
//红外调焦预置位指令（需配置）
#define SET_PRESET		0x0101
#define CLEAR_PRESET	0x0100
#define GOTO_PRESET		0x0111
#define GET_PRESETS		0x0121

//山火等的巡航路径命令
#define SHOW_ALL_TOURS		0x090C //显示所有预置路径信息，暂不实现
#define ADD_TOUR			0x090D //添加一条路径
#define REMOVE_TOUR			0x090E //删除一条路径
#define START_TOUR			0x090F //执行一条路径
#define STOP_TOUR			0x0912 //停止一条路径
#define GET_TOURS_NUM		0x0910 //获取预置路径数量
#define GET_TOUR_INFO	    0x0911 //获取一条路径的信息
#define REMOVE_TOURS		0x0913 //清空所有路径



#define DEFAULT_FLAG 0x646101BB
//包头，在每一个包发出去之前都要加这个头
typedef struct tag_M_HY_HEADER
{
unsigned long		flag;			// 标志:0x646101BB
unsigned long		command;	// 数据类型,请参见HYOC_def.h
unsigned long		len;			// 包长度,不含M_HY_HEADER长度
}M_HY_HEADER, *PM_HY_HEADER;


//登录、用户名验证
typedef struct tag_HY_LOGIN
{
	char  user_name[16];
	char  password[16];
} HY_LOGIN;

typedef struct tag_Dvr_INFO
{
	unsigned int dvr;//dvr编号以1开始
	unsigned int ipc_num;// 例如ipc数量为4，则ipc编号为1,2,3,4
//	char a9_type; // 0x01 云台A9, 0x02 主机A9
	float soft_version;//0.1
//	float hard_version;//0.1
	char data[64];//可显示字符，可以包含升级成功失败信息
} Dvr_INFO;

//打开实时流
typedef struct tag_REAL_TIME_REQUEST
{
	unsigned int	dvr;
	unsigned int	ipc;
} REAL_TIME_REQUEST;


//管理预置点.因为只有一个云台所以不需要ipc
typedef struct tag_PRESET
{
	unsigned int preset_type;//1调焦预置点 2普通预置点 3告警预置点
	unsigned int preset_id;
} PRESET;

typedef struct tag_PRESET_MANAGEMENT
{
	unsigned int	dvr;
	unsigned int	ipc;
	unsigned int    cmd;
	PRESET preset;
} PRESET_MANAGEMENT;

//巡航路径中的预置点信息为
typedef struct
{
    int	  id;             //预置点ID
	int   duration;   	  //预置点停留时间
	float speed;		 //移动至该预置点速度
}ov_preset_t;
//山火、普通视频、无人机巡航路径信息
typedef struct
{
	unsigned int	dvr;
	unsigned int	ipc;
	unsigned int    cmd;  //预置路径操作指令
	unsigned int    patrolId;	//预置路径编号
	ov_preset_t     presets[8];//路径中包含的预置点信息
	int             preset_num;   //预置点数量
}ptz_patrol_cmd_t;

typedef struct
{
	unsigned int	dvr;
	unsigned int	ipc;
	int patrolID; //按预置路径运行
	int runTime; //录像时长
}video_record_t;

typedef struct
{
	unsigned int	dvr;
	unsigned int	ipc;
	int presetId; //按预置点运行
	int picSize; //拍摄照片的大小，即清晰度
}picture_record_t;





/*调焦预置点操作*/
typedef struct
{
	unsigned int cmd;
	char name[32];//只能使用英文名，方便操作
} zoom_preset_cmd_t;

enum e_zoom_preset_cmd
{
	ZOOM_PRESET_ADD = 0x0101,
	ZOOM_PRESET_DEL = 0x0100,
	ZOOM_PRESET_GOTO = 0x0111,
	//ZOOM_PRESETS_SHOW = 4
};

//PTZ控制
typedef struct tag_SYS_PTZ
{
	unsigned int	dvr;
	unsigned int	ipc;
	unsigned int	cmd;
	union
	{
		unsigned char spead;//上下左右使用时的速度
		unsigned char point;//调用设置预置点时，预置点的位置
		unsigned char swith;//开关电，0x01开电，0x02关电
		unsigned char zoom;//变倍，0x01远，0x02近
		unsigned char focus;//焦距，0x01远，0x02近
	};
} SYS_PTZ;


//设置扫描范围
typedef struct tag_PTZ_SCAN_RANGE
{
    short horizon_start_turn;
    short horizon_stop_turn;
    short vertical_start_turn;
    short vertical_stop_turn;
} PTZ_SCAN_RANGE;

//
//数据通道结构体
//
//图片或者录像
//在传递告警图片和普通截图的时候
typedef struct
{
	unsigned int		ipc;
	unsigned int 		stream;      //在LOG的情况下，1为Alarm 0为Normal
	unsigned int 		size;		 //数据大小
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
} VEDIO_OR_PICTURE_DATA;//32，录像或图片

//场景信息
typedef struct
{
	unsigned int SceneID;//场景编号
	unsigned int HorizonDegree;//场景的水平角度
	unsigned int VerticalDegree;//场景的垂直角度
	unsigned int Row;//该场景需要划分为多少行
	unsigned int Column;//该场景需要划分为列
	unsigned int Size;
} SCENE_INFO;

typedef struct
{
	unsigned int		ipc;
	unsigned int 		stream;
	unsigned int        error;//错误码，0无错误，1 IPC不存在，2 IPC ping不通， 3 正在播放， 4 推流中断 ，5待定
} REAL_TIME_STREAM_DATA;//8，实时流数据


typedef struct
{
	int x;
	int y;
} MPOINT;

typedef struct
{
	int PresetID;//按预置位屏蔽
    int subFrameID;//按告警编号屏蔽
	int AngleFromNorth;
	int DistanceToDevice;
    MPOINT topLeftCorner;
    MPOINT lowerRightCorner;
} FIRE_REGION;

typedef struct
{
	int ipc;
	int stream;
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int type;			//野火：1——羽流，2——烟，3——羽流+烟；外破：11小汽车啊，12吊车啊，13挖掘机啊，14推土机啊等等
	int level;			//1——高级；2——一般；3——低级
	double area;		//野火：着火区域面积
	double hor_turn;	//野火：云台水平转角
	double ver_turn;	//野火：云台垂直转角
	double longitude;	//野火：着火区域经度
	double latitude;	//野火：着火区域纬度
	double distance;	//野火：着火区域与监测点之间距离
} ALARM_DATA;//28，报警数据

typedef struct
{
	int id;//mk60 ID
	char data[28];//28字符串携带升级成功失败信息
} MK60_UPDATE_DATA;//mk60升级信息

typedef struct
{
	int id;//暂时不用，云台ID
	char data[28];//28字符串携带升级成功失败信息
} MK60_PTZ_SACN_RANGE_DATA;//扫描范围设置成功或失败

// 系统状态结构体
typedef struct
{
	int ir_camera_status;//-1,网络异常;0,正常;
	int visu_camera_status;//-1,网络异常;0，正常
	int mobile_net_status;//3G网络状态-1，网络异常；0，正常
	int ttyUSB0_status;//3G模块串口状态，-1，串口被其它程序占用；0，正常
} DEVICE_STATUS;

//3G流量
typedef struct
{
	int year;
	int month;
	int flow; //流量值
} MOBILE_FLOW_VALUE;

typedef struct
{
	int 	srv_status; //服务状态
	int		sys_mode;	//系统模式
	int 	sim_state;	//sim状态

	int		rssi;		//md 1x信号强度
	int 	fer;		//ma 1x数据帧出错率
	int 	hdr_rssi;	//ma 3g信号强度
}CDMA_MODEL_STATUS;

//数据通道的结构体
typedef struct
{
	unsigned int flag;
	unsigned int type;
	unsigned int dvr;
	union
	{
		SCENE_INFO				scene_info;//场景信息
		VEDIO_OR_PICTURE_DATA 	pic_data;//图片或录像或日志
		REAL_TIME_STREAM_DATA 	stream;//实时流
		ALARM_DATA 				alarm;//
		MK60_UPDATE_DATA 		update;
		MK60_PTZ_SACN_RANGE_DATA scan_range_rsp;
		DEVICE_STATUS			device_status;
		MOBILE_FLOW_VALUE		mobile_flow_value;
		CDMA_MODEL_STATUS		cdma;
	};
} DATA_STRUCT;

typedef struct
{
	unsigned int	dvr;
	unsigned int	ipc;
	union
	{
		unsigned int	sub_frame_ID;//火源编号
		unsigned int	PresetID;
	};
	unsigned int	cmd;
	//0x00按sub_frame_ID取消正常火源 0x01按sub_frame_ID设置正常火源	 0x02按PresetID取消正常火源 0x03按PresetID设置正常火源
	//0x10——水体，如河流、湖泊、水库、池塘；
	//0x11——建筑，如村庄、城镇、公路；
	//0x12——农业区；
	//0x13——草地；
	//0x14——疏林，即夹杂着草地、树木的区域；
	//0x15 Mr Tseng 4/7/2015 1:08:07 PM
//0x15——密林

} NORMAL_FIRE;

typedef struct
{
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
} REAL_TIME;

typedef struct
{
	int AlarmValue;
} ALARM_PARAM;

typedef struct
{
	int StartHour;
	int StartMin;
	int StartSecond;
	int StopHour;
	int StopMin;
	int StopSecond;
} DETECT_TIME;


typedef struct _hy_mv_rect_t
{
	int left;
	int top;
	int right;
	int bottom;
} hy_mv_rect_t;

typedef struct _hy_channel_detect_area_t
{
	unsigned char	channel;	// 通道号
	unsigned char   delay;		// 0 关闭检测 ， 大于0 标志打开检测及报警延迟时间,单位：秒
	unsigned char	grade ;		// 运动检测的灵敏度,设定得值范围3-8--->input
	unsigned char	rectcount;	// 当前路视频实际的运动检测区域数量--->input
	float           w_h_scale;  // 宽高比
	float           Arescale;   // 宽高比
	hy_mv_rect_t		rect[16];//运动检测区域坐标--->input
} hy_channel_detect_area_t;

typedef struct _hy_algorithm_param_t
{
    int init_model;
} hy_algorithm_t;

typedef struct _hy_scene_parameter_t
{
    unsigned char channel;
    unsigned char preset;
    unsigned char re_construct;//场景重构标记
    hy_mv_rect_t wire_point[4];
    hy_mv_rect_t bound_point[4];
    hy_algorithm_t algorithm_param;
} hy_scene_parameter_t;


/* 相机参数配置 */
enum video_encoder_config
{
	/*以前的只用于admin:12345*/
	SET_352_288_6 = 1,
	SET_704_576_6 = 2,
	SET_704_576_10 = 3,
	SET_704_576_15 = 4,
	SET_704_576_20 = 5,

	/*新增加*/
	//图像质量3L、2L、1L、M、1H、2H，最低、较低、低、中、高、最高
	SET_352_288_6_12_M_256 = 6,
	SET_704_576_6_12_3L_512 = 7,
	SET_704_576_6_12_M_1024 =8,     //704*576,6帧/s,每12帧1个I帧，图像质量中等，码率1024kpbs
    SET_704_576_6_12_2H_1024 = 9
};
#endif

