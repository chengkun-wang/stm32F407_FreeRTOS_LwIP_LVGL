//#include "lvgl.h"  
//#include <stdio.h>  
//#include <stdlib.h>

//lv_obj_t * scr1, * scr2;  

//lv_coord_t  delta_x,delta_y;

//// 触摸开始时的坐标  
//static lv_point_t drag_start_point;  
//static lv_point_t drag_end_point; 
//// 触摸事件处理函数  
//static void scr_event_handler(lv_event_t * e) 
//{  
//    lv_event_code_t code = lv_event_get_code(e);  
//    lv_obj_t * scr = lv_event_get_target(e);  
//	
//    if (code == LV_EVENT_PRESSED) 
//	{  
//        // 记录触摸开始时的坐标  
//        lv_indev_t * indev1 = lv_indev_get_act();  
//        lv_indev_get_point(indev1, &drag_start_point);  
//    } 
//	else if (code == LV_EVENT_RELEASED) 
//	{  
//		//记录触摸结束时的坐标
//        lv_indev_t * indev2 = lv_indev_get_act();  
//        lv_indev_get_point(indev2, &drag_start_point); 
//		// 在这里，你可以计算滑动方向和距离	
//		delta_x = abs(drag_start_point.x - drag_end_point.x);
//		delta_y = abs(drag_start_point.y - drag_end_point.y);
//		
//		if (delta_x>delta_y && delta_x>5)
//		{
//			lv_scr_load_anim(scr2, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, true); 
//		}
//        // 如果向右滑动且距离足够，则准备切换页面  
//    }
//        // 示例：假设我们决定切换页面  
//	
//        // 重置或更新状态（如果需要）  
//      
//}  
//  
//// 在某个初始化函数中设置事件监听器  
//void setup_pages(lv_obj_t* Page) 
//{  
//    scr1 = lv_obj_create(NULL);
//	
//	scr2 = lv_obj_create(NULL);
//    lv_obj_add_event_cb(scr1, scr_event_handler, LV_EVENT_PRESSED|LV_EVENT_RELEASED, NULL);  
//  
//}  
//  