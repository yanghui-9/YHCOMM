#ifndef _IOP_DEF_H_
#define _IOP_DEF_H_

#include <mutex>
#include <vector>
#include <list>
#include "io_config.h"

#define IOP_ERR_SOCKET -1
#define IOP_ERR_NO_MEM -2
#define IOP_ERR_PROTOCOL -3
#define IOP_ERR_TIMEOUT -4
#define IOP_ERR_CLOSED -5

//���¼�
#define EV_TYPE_READ                1
//д�¼�
#define EV_TYPE_WRITE               2
//�����¼�
#define EV_TYPE_CREATE              4
//�����¼�
#define EV_TYPE_DESTROY             8
//��ʱ���¼�
#define EV_TYPE_TIMER               16
//��ʱ(�ͻ��˻�Ծ��ʱ)
#define EV_TYPE_TIMEOUT				32
//�����¼�
#define EV_TYPE_ERROR               64
//���Ӵ����¼�
#define EV_TYPE_CONNECT             128
#define IOP_ERR_SYS  -1

//һ������¼����͵ĺ�
#define EV_IS_SET_R(e) ((e)&EV_TYPE_READ)
#define EV_IS_SET_W(e) ((e)&EV_TYPE_WRITE)
#define EV_IS_SET(e,x) ((e)&x)

#define EV_SET_R(e) ((e) | EV_TYPE_READ)
#define EV_CLR_R(e) ((e) & ~EV_TYPE_READ)
#define EV_SET_W(e) ((e) | EV_TYPE_WRITE)
#define EV_CLR_W(e) ((e) & ~EV_TYPE_WRITE)

#define EV_SET_RW(e) ((e)|EV_TYPE_READ|EV_TYPE_WRITE)
#define EV_TYPE_RW (EV_TYPE_READ|EV_TYPE_WRITE)

//ϵͳ֧�ֵ��¼�ģ��
//NULL������ϵͳ�Զ�ѡ��
#define IOP_MODEL_NULL                      0
//event portsģ����δʵ��
#define IOP_MODEL_EVENT_PORTS       1
//kqueueģ����δʵ��
#define IOP_MODEL_KQUEUE                  2
//linux epollģ���Ѿ�֧��
#define IOP_MODEL_EPOLL                     3
//dev pollģ����δʵ��
#define IOP_MODEL_DEV_POLL              4
//pollģ���Ѿ�֧��
#define IOP_MODEL_POLL                       5
//select ģ���Ѿ�֧��. windows & linux
#define IOP_MODEL_SELECT                   6
//windows iocpģ����δʵ��
#define IOP_MODEL_IOCP                       7
//ֻ֧�ֶ�ʱ��
#define IOP_MODEL_TIMER                     8

#define DEFAULT_DISPATCH_INTERVAL	500

/*max socket data buf:64M*/
#define IOP_MAX_BUF_SIZE   67108864
#define IOP_TYPE_FREE	0
#define IOP_TYPE_IO		1
#define IOP_TYPE_TIMER	2
#define IOP_TYPE_CONNECT	3



struct io_base_t;
struct io_t;
typedef int (*io_event_cb)(io_base_t *,int,unsigned int,void *);
typedef time_t io_time_t;

typedef void (*base_free)(io_base_t *);      //��Դ�ͷŵĽӿ�
typedef int (*base_dispatch)(io_base_t *, int);  //ģ�͵��Ƚӿ�
typedef int (*base_add)(io_base_t *, int, io_handle_t, unsigned int);//����¼�
typedef int (*base_del)(io_base_t *, int,io_handle_t);//ɾ���¼�
typedef int (*base_mod)(io_base_t *, int, io_handle_t, unsigned int);//�޸��¼�

using ClientEventCB = std::function<void(io_t &,unsigned int)>;
using ServerEventCB = std::function<void(unsigned int)>;

struct io_op_t
{
    std::string name;    //ģ������
    base_free _base_free;//��Դ�ͷŵĽӿ�
    base_dispatch _base_dispatch;//ģ�͵��Ƚӿ�
    base_add _base_add;//����¼�
    base_del _base_del;//ɾ���¼�
    base_mod _base_mod;   //�޸��¼�
};
struct io_t
{
    io_handle_t handle;		/*�����ľ��*/
    int timeout;			/*��ʱֵ*/
    io_event_cb evcb;		/*�¼��ص�*/
    std::vector<char> sbuf;		/*���ͻ�����*/
    std::vector<char> rbuf;		/*���ջ�����*/
    io_time_t last_dispatch_time;	/*�ϴε��ȵ�ʱ��*/
    int port;
    std::string  sIp;

    bool operator == (const io_t &iop) const
    {
        if(handle == iop.handle)
        {
            return true;
        }
        return false;
    }
};

struct io_base_t
{
    io_handle_t handle;
    std::list<io_t> iops;		/*����iop*/
    int maxio;			/*��󲢷���*/
    uint32_t maxbuf;			/*�������ͻ���ջ�������ֵ*/

    int dispatch_interval;		/*select�ĳ�ʱʱ��*/

    io_op_t op_imp;           /*�¼�ģ�͵��ڲ�ʵ��*/
    void *model_data;         /*�¼�ģ���ض�������*/

    ServerEventCB serverEventCB; /*������¼��ص�*/
    ClientEventCB clientEventCB; /*�ͻ����¼��ص�*/

    io_time_t cur_time;		/*��ǰ����ʱ��*/
    io_time_t last_time;		/*�ϴε���ʱ��*/
    io_time_t last_keepalive_time; /*�ϴμ��keepalive��ʱ��*/
    std::mutex comm_lock; //��
};

#define IOP_CB(base,iop,events)	{{if(0==(iop->evcb)(base, iop->id,events,iop->arg)){iop->last_dispatch_time = base->cur_time;}else{io_del(base,iop->id);}}}

#endif


