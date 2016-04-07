/*************************************
 *����ͨѶ ���ݽṹ
 *date:     2014.10.22
 *changelog:
 *
 *************************************/

#ifndef _HY_PROTOCL_H
#define _HY_PROTOCL_H

//����ͨ��
//�·���ʱ���ȷ�ͷ���ٷ�����
//���յ�ʱ��ֻ����ͷ
//1.	��������len = 0  ɽ��&����
#define ATOD_KEEP_HEART_BEAT		0x61646301
#define DTOA_KEEP_HEART_BEAT		0x64616301
//2.	ʵʱ����len = 8  ɽ��&����
#define ATOD_REAL_TIME_REQUEST		0x61646302
#define DTOA_REAL_TIME_RSP			0x64616302
//3.	ptz,��len = 12  ɽ��&����
#define ATOD_SYS_PTZ					0x61646303
#define DTOA_SYS_PTZ					0x64616303
//4.	����,��len = 0
#define ATOD_SYS_UPDATE				0x61646304//ɽ��&���ƴ���ƽ̨����
#define DTOA_SYS_UPDATE				0x64616304//ɽ��&���ƴ���ƽ̨����
#define ATOD_MK60_1_UPDATE			0x61646305//MK60 1 ���� ��A9ֱ�����ӵ�MK60
#define DTOA_MK60_1_UPDATE			0x64616305//MK60 1 ����
#define ATOD_MK60_2_UPDATE			0x61646311//MK60 2 ����
#define DTOA_MK60_2_UPDATE			0x64616311//MK60 2 ����
#define ATOD_MK60_EMER_UPDATE	    0x61646314//mk60Ӧ������
#define DTOA_MK60_EMER_UPDATE		0x64616314
#define ATOD_PTZ_MANU				0x61646306//ptz�ֶ�ָ��,ɽ��&����
#define DTOA_PTZ_MANU				0x64616306
#define ATOD_PTZ_AUTO				0x61646307//ptz�Զ�ָ��,ɽ��&����
#define DTOA_PTZ_AUTO				0x64616307
//5.	A9����(��߲���Ҫ��)
#define	ATOD_A9_RESTART				0x61646308//A9Reboot,ɽ��&����
#define	DTOA_A9_RESTART				0x64616308//A9Reboot,ɽ��&����
//6.	����ɨ�跶Χ
#define ATOD_PTZ_RANGE		0x61646309
#define	DTOA_PTZ_RANGE		0x64616309
//7.	��Դ����
#define ATOD_CONFIG_NORMAL_FIRE		0x61646310
#define DTOA_CONFIG_NORMAL_FIRE		0x64616310
//8.	�·�ϵͳʱ��
#define ATOD_SET_ARM_REAL_TIME		0x61646312//ɽ��&����
#define DTOA_SET_ARM_REAL_TIME		0x64616312//ɽ��&����
//9.	����Ԥ�õ�
#define ATOD_MANAGE_PRESET			0x61646313//ɽ��&����
#define DTOA_MANAGE_PRESET			0x64616313//ɽ��&����
//10.	���ú���̽��澯��ֵ
#define ATOD_SET_AlARM_PARAM		0x61646315
#define DTOA_SET_ALARM_PARAM		0x64616315
//11.	3G����ͳ��
#define ATOD_MOBILE_FLOW_VALUE		0x61646316//��ȡDVRͳ�Ƶ�3g����,ɽ��&����
#define DTOA_MOBILE_FLOW_VALUE		0x64616316//ɽ��&����
//12.	���������Զ��۽�
#define ATOD_SET_IR_FOCUS			0x61646317
#define DTOA_SET_IR_FOCUS			0x64616317
#define ATOD_SET_IR_FOCUS_ADD		0x61646318//��������ֶ��۽�+
#define DTOA_SET_IR_FOCUS_ADD		0x64616318
#define ATOD_SET_IR_FOCUS_SUB		0x61646319//��������ֶ��۽�-
#define DTOA_SET_IR_FOCUS_SUB		0x64616319
#define	ATOD_GET_IR_PHOTO			0x61646320//��ȡ�������ͼƬ
#define	DTOA_GET_IR_PHOTO			0x64616320
#define	ATOD_IR_FOCUS_ADJUST		0x61646321//��ȡ�������ͼƬ
#define	DTOA_IR_FOCUS_ADJUST		0x64616321
#define	ATOD_SET_DETECT_TIME		0x61646322//����̽��ʱ��
#define	DTOA_SET_DETECT_TIME		0x64616322
#define	ATOD_SET_VIDEO_CONFIG		0x61646323//������Ƶ�������
#define	DTOA_SET_VIDEO_CONFIG		0x64616323
//13.	�ɼ������Ԥ��λ���鿴�ط�ʱʹ�ã�
#define	ATOD_SET_ZOOM_PRESET		0x61646324
#define	DTOA_SET_ZOOM_PRESET		0x64616324
//14.	��ȡ�ɼ������ͼƬ
#define	ATOD_GET_VISU_PHOTO			0x61646325//��ȡ�ɼ������ͼƬ
#define	DTOA_GET_VISU_PHOTO			0x64616325
//15.	���ú���̽������澯��ֵ
#define ATOD_SET_AlARM_AREA_PARAM	0x61646326
#define DTOA_SET_ALARM_AREA_PARAM	0x64616326
//16.	���ó�������
#define ATOD_SET_SCENE_TYPE			0x61646327
#define DTOA_SET_SCENE_TYPE			0x64616327
//17.	��ȡ����
#define ATOD_GET_SCENE				0x61646328
#define DTOA_GET_SCENE				0x64616328
//18.	ping�ɼ��������
#define ATOD_PING_CAMERA			0x61646329//ɽ��&����
#define DTOA_PING_CAMERA			0x64616329//ɽ��&����
//19.	ɽ��ȵ�Ѳ��·��
#define	ATOD_PTZ_TOUR				0x61646331//onvifԤ��·������
#define	DTOA_PTZ_TOUR				0x64616331
//20.    �ƻ�����
#define	ATOD_VIDEO_RECORD_TASK		0x61646332//�ƻ�¼������
#define	DTOA_VIDEO_RECORD_TASK		0x64616332
#define	ATOD_PICTURE_RECORD_TASK	0x61646333//�ƻ���ͼ����
#define	DTOA_PICTURE_RECORD_TASK	0x64616333
//22.  ͬ���������ʱ��
#define	ATOD_SYNC_CAMERA_TIME		0x61646335
#define	DTOA_SYNC_CAMERA_TIME		0x64616335

//21.    �����ĸ������ļ����䣬registrat����ӳ�У׼
#define	ATOD_REGISTRATION_PARAMS	0x61646334//ͼƬ�غϲ���
#define	DTOA_REGISTRATION_PARAMS	0x64616334
//23.  ��ֵ�ļ�����
#define	 ATOD_THRESHOTLD_FILE_DOWNLOAD 0x61646336
#define	 DTOA_THRESHOTLD_FILE_DOWNLOAD 0x64616336
//24.  ������Ϣ���ݿ�����
#define	 ATOD_GEOPOS_FILE_DOWNLOAD	 0x61646337
#define	 DTOA_GEOPOS_FILE_DOWNLOAD	 0x64616337
//25.  ���������
#define	 ATOD_LINUX_LIB_DOWNLOAD	 0x61646338
#define	 DTOA_LINUX_LIB_DOWNLOAD	 0x64616338
//26.  �ر�ʵʱ�������� 2015-8-14
#define 	ATOD_REAL_STREAM_STOP	0x61646339//
#define 	DTOA_REAL_STREAM_STOP	0x64616339
//27.  �������������������2015-8-14 Jimmy
#define 	ATOD_SET_DETECT_AREA	0x61646343
#define 	DTOA_SET_DETECT_AREA	0x64616343
//28.  ��ȡ�����������,����2015-8-24 Jimmy
#define     ATOD_GET_DETECT_AREA    0x61646344
#define 	DTOA_GET_DETECT_AREA	0x64616344

//����ͨ��
//1��	ͼƬ,ɽ��&����
#define DTOA_PICTURE_INFO				0x64616901
//2��	¼��,ɽ��&����
#define DTOA_RECORD_FILE_INFO			0x64616902
//3��	ʵʱ��
#define DTOA_REAL_TIME_STREAM_INFO		0x64616903
//4��	������Ϣ,ɽ��&����
#define DTOA_ALARM_INFO					0x64616904
//5.	������Ϣ
#define DTOA_MK60_1_UPDATE_INFO		    0x64616905//mk60������Ϣ�����Ǵ���ƽ̨��������Ϣ��ÿ��������ʱ��������ͨ�����͸����������
//6.	ɨ�跶Χ������Ӧ
#define DTOA_PTZ_RANGE_INFO		        0x64616906//mk60 ptz range��Ϣ
//7.	�ɼ���ͼƬ
#define DTOA_VISU_PICTURE_INFO			0x64616907//
//8.	�豸״̬,ɽ��&����
#define DTOA_DEVICE_STATUS_INFO			0x64616908
//9.	Dvr��־
#define DTOA_LOG_INFO					0x64616909
//10.	3g����,ɽ��&����
#define DTOA_MOBILE_FLOW_INFO     		0x6461690A
//11.	3Gģ��״̬,ɽ��&����
#define DTOA_3G_MODULE_STATUS     		0x6461690B
//12.	Dvr���ݿ�
#define DTOA_DB_INFO					0x6461690C
//13.	Dvr����ͼƬ
#define DTOA_SCENE_INFO			        0x6461690E
//14.	Dvr�ɼ��ⳡ��ͼƬ
#define DTOA_VISU_SCENE_INFO			0x6461690F
//15.    �ƻ�����
#define DTOA_VEDIO_TASK_INFO		0x64616911//�ƻ�¼��
#define	DTOA_PICTURE_TASK_INFO		0x64616912//�ƻ���ͼ

//PTZָ��
#define MOVE_UP 0x0402
#define MOVE_L_UP 0x0702
#define MOVE_R_UP 0x0802
#define MOVE_DOWN 0x0404
#define MOVE_L_DOWN 0x0704
#define MOVE_R_DOWN 0x0804
#define MOVE_L 0x0504
#define MOVE_R 0x0502
#define MOVE_STOP 0x0901
#define VISU_CAM_SWITH	    	0x0902 //�ɼ������������
#define IR_CAM_SWITH	    	0x0903 //�������������
#define A9_POWER_OFF			0x0904 //A9�ϵ�
#define VISU_CAM_ZOOM			0x0905 //�ɼ���䱶
#define VISU_CAM_ZOOM_STOP      0x0906 //�ɼ��䱶ֹͣ
#define VISU_CAM_FOCUS			0x0907 //�ɼ�������۽� 0x01�۽��� 0x02�۽�Զ
#define VISU_CAM_FOCUS_STOP		0x0906 //�ɼ�������۽� ֹͣ
#define ZOOM_IN 0x0302//����
#define ZOOM_OUT 0x0304//����
#define ZOOM_IN_STOP 0x0305
#define ZOOM_OUT_STOP 0x0306
#define FOCUS_IN 0x0202
#define FOCUS_OUT 0x0204
//�������Ԥ��λָ������ã�
#define SET_PRESET		0x0101
#define CLEAR_PRESET	0x0100
#define GOTO_PRESET		0x0111
#define GET_PRESETS		0x0121

//ɽ��ȵ�Ѳ��·������
#define SHOW_ALL_TOURS		0x090C //��ʾ����Ԥ��·����Ϣ���ݲ�ʵ��
#define ADD_TOUR			0x090D //���һ��·��
#define REMOVE_TOUR			0x090E //ɾ��һ��·��
#define START_TOUR			0x090F //ִ��һ��·��
#define STOP_TOUR			0x0912 //ֹͣһ��·��
#define GET_TOURS_NUM		0x0910 //��ȡԤ��·������
#define GET_TOUR_INFO	    0x0911 //��ȡһ��·������Ϣ
#define REMOVE_TOURS		0x0913 //�������·��



#define DEFAULT_FLAG 0x646101BB
//��ͷ����ÿһ��������ȥ֮ǰ��Ҫ�����ͷ
typedef struct tag_M_HY_HEADER
{
unsigned long		flag;			// ��־:0x646101BB
unsigned long		command;	// ��������,��μ�HYOC_def.h
unsigned long		len;			// ������,����M_HY_HEADER����
}M_HY_HEADER, *PM_HY_HEADER;


//��¼���û�����֤
typedef struct tag_HY_LOGIN
{
	char  user_name[16];
	char  password[16];
} HY_LOGIN;

typedef struct tag_Dvr_INFO
{
	unsigned int dvr;//dvr�����1��ʼ
	unsigned int ipc_num;// ����ipc����Ϊ4����ipc���Ϊ1,2,3,4
//	char a9_type; // 0x01 ��̨A9, 0x02 ����A9
	float soft_version;//0.1
//	float hard_version;//0.1
	char data[64];//����ʾ�ַ������԰��������ɹ�ʧ����Ϣ
} Dvr_INFO;

//��ʵʱ��
typedef struct tag_REAL_TIME_REQUEST
{
	unsigned int	dvr;
	unsigned int	ipc;
} REAL_TIME_REQUEST;


//����Ԥ�õ�.��Ϊֻ��һ����̨���Բ���Ҫipc
typedef struct tag_PRESET
{
	unsigned int preset_type;//1����Ԥ�õ� 2��ͨԤ�õ� 3�澯Ԥ�õ�
	unsigned int preset_id;
} PRESET;

typedef struct tag_PRESET_MANAGEMENT
{
	unsigned int	dvr;
	unsigned int	ipc;
	unsigned int    cmd;
	PRESET preset;
} PRESET_MANAGEMENT;

//Ѳ��·���е�Ԥ�õ���ϢΪ
typedef struct
{
    int	  id;             //Ԥ�õ�ID
	int   duration;   	  //Ԥ�õ�ͣ��ʱ��
	float speed;		 //�ƶ�����Ԥ�õ��ٶ�
}ov_preset_t;
//ɽ����ͨ��Ƶ�����˻�Ѳ��·����Ϣ
typedef struct
{
	unsigned int	dvr;
	unsigned int	ipc;
	unsigned int    cmd;  //Ԥ��·������ָ��
	unsigned int    patrolId;	//Ԥ��·�����
	ov_preset_t     presets[8];//·���а�����Ԥ�õ���Ϣ
	int             preset_num;   //Ԥ�õ�����
}ptz_patrol_cmd_t;

typedef struct
{
	unsigned int	dvr;
	unsigned int	ipc;
	int patrolID; //��Ԥ��·������
	int runTime; //¼��ʱ��
}video_record_t;

typedef struct
{
	unsigned int	dvr;
	unsigned int	ipc;
	int presetId; //��Ԥ�õ�����
	int picSize; //������Ƭ�Ĵ�С����������
}picture_record_t;





/*����Ԥ�õ����*/
typedef struct
{
	unsigned int cmd;
	char name[32];//ֻ��ʹ��Ӣ�������������
} zoom_preset_cmd_t;

enum e_zoom_preset_cmd
{
	ZOOM_PRESET_ADD = 0x0101,
	ZOOM_PRESET_DEL = 0x0100,
	ZOOM_PRESET_GOTO = 0x0111,
	//ZOOM_PRESETS_SHOW = 4
};

//PTZ����
typedef struct tag_SYS_PTZ
{
	unsigned int	dvr;
	unsigned int	ipc;
	unsigned int	cmd;
	union
	{
		unsigned char spead;//��������ʹ��ʱ���ٶ�
		unsigned char point;//��������Ԥ�õ�ʱ��Ԥ�õ��λ��
		unsigned char swith;//���ص磬0x01���磬0x02�ص�
		unsigned char zoom;//�䱶��0x01Զ��0x02��
		unsigned char focus;//���࣬0x01Զ��0x02��
	};
} SYS_PTZ;


//����ɨ�跶Χ
typedef struct tag_PTZ_SCAN_RANGE
{
    short horizon_start_turn;
    short horizon_stop_turn;
    short vertical_start_turn;
    short vertical_stop_turn;
} PTZ_SCAN_RANGE;

//
//����ͨ���ṹ��
//
//ͼƬ����¼��
//�ڴ��ݸ澯ͼƬ����ͨ��ͼ��ʱ��
typedef struct
{
	unsigned int		ipc;
	unsigned int 		stream;      //��LOG������£�1ΪAlarm 0ΪNormal
	unsigned int 		size;		 //���ݴ�С
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
} VEDIO_OR_PICTURE_DATA;//32��¼���ͼƬ

//������Ϣ
typedef struct
{
	unsigned int SceneID;//�������
	unsigned int HorizonDegree;//������ˮƽ�Ƕ�
	unsigned int VerticalDegree;//�����Ĵ�ֱ�Ƕ�
	unsigned int Row;//�ó�����Ҫ����Ϊ������
	unsigned int Column;//�ó�����Ҫ����Ϊ��
	unsigned int Size;
} SCENE_INFO;

typedef struct
{
	unsigned int		ipc;
	unsigned int 		stream;
	unsigned int        error;//�����룬0�޴���1 IPC�����ڣ�2 IPC ping��ͨ�� 3 ���ڲ��ţ� 4 �����ж� ��5����
} REAL_TIME_STREAM_DATA;//8��ʵʱ������


typedef struct
{
	int x;
	int y;
} MPOINT;

typedef struct
{
	int PresetID;//��Ԥ��λ����
    int subFrameID;//���澯�������
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
	int type;			//Ұ��1����������2�����̣�3��������+�̣����ƣ�11С��������12��������13�ھ������14���������ȵ�
	int level;			//1�����߼���2����һ�㣻3�����ͼ�
	double area;		//Ұ���Ż��������
	double hor_turn;	//Ұ����̨ˮƽת��
	double ver_turn;	//Ұ����̨��ֱת��
	double longitude;	//Ұ���Ż����򾭶�
	double latitude;	//Ұ���Ż�����γ��
	double distance;	//Ұ���Ż����������֮�����
} ALARM_DATA;//28����������

typedef struct
{
	int id;//mk60 ID
	char data[28];//28�ַ���Я�������ɹ�ʧ����Ϣ
} MK60_UPDATE_DATA;//mk60������Ϣ

typedef struct
{
	int id;//��ʱ���ã���̨ID
	char data[28];//28�ַ���Я�������ɹ�ʧ����Ϣ
} MK60_PTZ_SACN_RANGE_DATA;//ɨ�跶Χ���óɹ���ʧ��

// ϵͳ״̬�ṹ��
typedef struct
{
	int ir_camera_status;//-1,�����쳣;0,����;
	int visu_camera_status;//-1,�����쳣;0������
	int mobile_net_status;//3G����״̬-1�������쳣��0������
	int ttyUSB0_status;//3Gģ�鴮��״̬��-1�����ڱ���������ռ�ã�0������
} DEVICE_STATUS;

//3G����
typedef struct
{
	int year;
	int month;
	int flow; //����ֵ
} MOBILE_FLOW_VALUE;

typedef struct
{
	int 	srv_status; //����״̬
	int		sys_mode;	//ϵͳģʽ
	int 	sim_state;	//sim״̬

	int		rssi;		//md 1x�ź�ǿ��
	int 	fer;		//ma 1x����֡������
	int 	hdr_rssi;	//ma 3g�ź�ǿ��
}CDMA_MODEL_STATUS;

//����ͨ���Ľṹ��
typedef struct
{
	unsigned int flag;
	unsigned int type;
	unsigned int dvr;
	union
	{
		SCENE_INFO				scene_info;//������Ϣ
		VEDIO_OR_PICTURE_DATA 	pic_data;//ͼƬ��¼�����־
		REAL_TIME_STREAM_DATA 	stream;//ʵʱ��
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
		unsigned int	sub_frame_ID;//��Դ���
		unsigned int	PresetID;
	};
	unsigned int	cmd;
	//0x00��sub_frame_IDȡ��������Դ 0x01��sub_frame_ID����������Դ	 0x02��PresetIDȡ��������Դ 0x03��PresetID����������Դ
	//0x10����ˮ�壬�������������ˮ�⡢������
	//0x11�������������ׯ�����򡢹�·��
	//0x12����ũҵ����
	//0x13�����ݵأ�
	//0x14�������֣��������Ųݵء���ľ������
	//0x15 Mr Tseng 4/7/2015 1:08:07 PM
//0x15��������

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
	unsigned char	channel;	// ͨ����
	unsigned char   delay;		// 0 �رռ�� �� ����0 ��־�򿪼�⼰�����ӳ�ʱ��,��λ����
	unsigned char	grade ;		// �˶�����������,�趨��ֵ��Χ3-8--->input
	unsigned char	rectcount;	// ��ǰ·��Ƶʵ�ʵ��˶������������--->input
	float           w_h_scale;  // ��߱�
	float           Arescale;   // ��߱�
	hy_mv_rect_t		rect[16];//�˶������������--->input
} hy_channel_detect_area_t;

typedef struct _hy_algorithm_param_t
{
    int init_model;
} hy_algorithm_t;

typedef struct _hy_scene_parameter_t
{
    unsigned char channel;
    unsigned char preset;
    unsigned char re_construct;//�����ع����
    hy_mv_rect_t wire_point[4];
    hy_mv_rect_t bound_point[4];
    hy_algorithm_t algorithm_param;
} hy_scene_parameter_t;


/* ����������� */
enum video_encoder_config
{
	/*��ǰ��ֻ����admin:12345*/
	SET_352_288_6 = 1,
	SET_704_576_6 = 2,
	SET_704_576_10 = 3,
	SET_704_576_15 = 4,
	SET_704_576_20 = 5,

	/*������*/
	//ͼ������3L��2L��1L��M��1H��2H����͡��ϵ͡��͡��С��ߡ����
	SET_352_288_6_12_M_256 = 6,
	SET_704_576_6_12_3L_512 = 7,
	SET_704_576_6_12_M_1024 =8,     //704*576,6֡/s,ÿ12֡1��I֡��ͼ�������еȣ�����1024kpbs
    SET_704_576_6_12_2H_1024 = 9
};
#endif

