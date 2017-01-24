/**
* @file D:\Documents\Projects\chargersdk\src\middlewares\userlib\src\libobserver.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-13
*/
#include <cstl/clist.h>

// �۲��߻ص�
typedef struct
{
    void (*update)(int);
} observer_t;

// ���۲���
typedef struct _subject_t
{
    int data;                 // ���۲�����
    list_t *observer_list; // �۲�������
} subject_t;

void update_form(int data)
{
    xprintf("update form~~~\n");
}

void update_cylinder(int data)
{
    xprintf("update cylinder~~~\n");
}

void update_percent(int data)
{
    xprintf("update percent~~~ yeah rgw!!!\n");
}

void init_observer(observer_t *observer, void (*update)(int))
{
    observer->update = update;
}

// ����۲��߶���
int attach_observer(subject_t *subject, observer_t *observer)
{
    // ��observer����subject�е�observer����

}

// �ӹ۲��߶���ɾ��
int detach_observer(subject_t *subject, observer_t *observer)
{
    // ��observer�߳�subject�е�observer����
}

// �޸ı��۲�����
int set_data(subject_t *subject, int value)
{
    subject->data = value;
}

// ֪ͨ�۲���
void notify_observer(subject_t *subject)
{
    list_t *node = subject->observer_list;

    for(; NULL != node; node = node->next)
    {
        node->observer->update(subject->data);
    }
}

// ��ʼ���������
void init_subject(subject_t *subject)
{
    subject->data = 0;
    list_init(subject->observer_list);
}

int main(int argc, const char *agrc[])
{
    observer_t form;        // ������
    observer_t cylinder;    // ��״ͼ����
    observer_t percent;     // �ٷֱȶ���
    subject_t subject;      // ͳ������

    // ��ʼ���۲���
    init_observer(&form, updateform);
    init_observer(&cylinder, updatecylinder);
    init_observer(&percent, updatepercent);

    // ��ʼ�����۲���
    init_subject(&subject);

    // �۲��߹�עsubject
    attach_observer(&subject, &form);
    attach_observer(&subject, &cylinder);
    attach_observer(&subject, &percent);

    // �޸�״̬����֪ͨ�۲���
    set_data(&subject, 1);
    notify_observer(&subject);

    set_data(&subject, 2);
    notify_observer(&subject);

    return 0;
}
