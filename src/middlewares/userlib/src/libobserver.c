/**
* @file D:\Documents\Projects\chargersdk\src\middlewares\userlib\src\libobserver.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-13
*/
#include <cstl/clist.h>

// 观察者回调
typedef struct
{
    void (*update)(int);
} observer_t;

// 被观察者
typedef struct _subject_t
{
    int data;                 // 被观察数据
    list_t *observer_list; // 观察者链表
} subject_t;

void update_form(int data)
{
    printf("update form~~~\n");
}

void update_cylinder(int data)
{
    printf("update cylinder~~~\n");
}

void update_percent(int data)
{
    printf("update percent~~~ yeah rgw!!!\n");
}

void init_observer(observer_t *observer, void (*update)(int))
{
    observer->update = update;
}

// 加入观察者队列
int attach_observer(subject_t *subject, observer_t *observer)
{
    // 将observer加入subject中的observer链表

}

// 从观察者队列删除
int detach_observer(subject_t *subject, observer_t *observer)
{
    // 将observer踢出subject中的observer链表
}

// 修改被观察数据
int set_data(subject_t *subject, int value)
{
    subject->data = value;
}

// 通知观察者
void notify_observer(subject_t *subject)
{
    list_t *node = subject->observer_list;

    for(; NULL != node; node = node->next)
    {
        node->observer->update(subject->data);
    }
}

// 初始化主题对象
void init_subject(subject_t *subject)
{
    subject->data = 0;
    list_init(subject->observer_list);
}

int main(int argc, const char *agrc[])
{
    observer_t form;        // 表格对象
    observer_t cylinder;    // 柱状图对象
    observer_t percent;     // 百分比对象
    subject_t subject;      // 统计数据

    // 初始化观察者
    init_observer(&form, updateform);
    init_observer(&cylinder, updatecylinder);
    init_observer(&percent, updatepercent);

    // 初始化被观察者
    init_subject(&subject);

    // 观察者关注subject
    attach_observer(&subject, &form);
    attach_observer(&subject, &cylinder);
    attach_observer(&subject, &percent);

    // 修改状态，并通知观察者
    set_data(&subject, 1);
    notify_observer(&subject);

    set_data(&subject, 2);
    notify_observer(&subject);

    return 0;
}
