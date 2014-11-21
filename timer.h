#ifndef TIMER_H
#define TIMER_H
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <map>
#include <vector>
#include <set>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
static uint32_t TIMER_INTERVAL_ID = 0;
class Timer
{
public:
	class DelayAction
	{
	public:
		DelayAction(boost::function<void()> func, int64_t timeout, int64_t interval) 
			: cb_func(func), m_tick(timeout), m_interval(interval), m_actionId(++TIMER_INTERVAL_ID){}
		void operator()()
		{
			cb_func();
		}
		int64_t tick() const 
		{
			return m_tick;
		}
		int64_t interval()
		{
			return m_interval;
		}
		void ResetTimeout(int timeout) 
		{
			m_tick = timeout;
		}
		
		uint32_t GetTimerId() const
		{
			return m_actionId;
		}
	public:
		uint32_t m_actionId;
		int64_t m_tick;
		int64_t m_interval;
		boost::function<void()> cb_func;
	};
	typedef std::multimap<int, DelayAction*> timer_map_t;
public:
	static Timer * GetInstance(){
		static Timer _instance;
		return &_instance;
	}
	void Update(int64_t tick)
	{	
		if (m_delayqueue.empty()) 
		{
			return;
		}
		for(timer_map_t::iterator it = m_delayqueue.begin(); it != m_delayqueue.end();) 
		{
			if (it->first > tick) 
			{
				break;
			} 
			else 
			{
				(*it->second)();
				if (it->second->interval() > 0) 
				{
					std::set<uint32_t>::iterator iter = clearIntervals_.find(it->second->GetTimerId());
					if(iter != clearIntervals_.end())
					{
						clearIntervals_.erase(iter);
					}
					else
					{
						intervals_.push_back(it->second);
					}
				} 
				else 
				{
					delete it->second;
				}
				m_delayqueue.erase(it++);
			}
		}
		if (!intervals_.empty()) 
		{
			for (std::vector<DelayAction*>::iterator it = intervals_.begin(); it != intervals_.end(); ++it) 
			{	
				(*it)->ResetTimeout(tick + (*it)->interval());
				m_delayqueue.insert(std::make_pair((*it)->tick(), *it));
			}
			intervals_.clear();
		}
	}
	uint32_t SetInterval(boost::function< void() > func, int interval)
	{
		int timeout = interval + time(NULL);
		m_delayqueue.insert(std::make_pair(timeout, new DelayAction(func, timeout, interval)));
		return TIMER_INTERVAL_ID;
	}
	
	void ClearInterval(uint32_t timerId)
	{
		clearIntervals_.insert(timerId);
	}
	void SetTimeout(boost::function<void()> func, int timeout)
	{
		timeout += time(NULL);
		m_delayqueue.insert(std::make_pair(timeout, new DelayAction(func, timeout, 0)));
	}
private:
	timer_map_t m_delayqueue;
	std::vector<DelayAction*> intervals_;
	std::set<uint32_t> clearIntervals_;
private:
	Timer(){}
};
#endif
