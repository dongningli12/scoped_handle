// Copyright (c) 2018 Dongning Li. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be found in the LICENSE file.

#ifndef SCOPED_HANDLE_H
#define SCOPED_HANDLE_H
//The motivation is to support RAII (resource acquisation is initialization) for handles to let a smart handle free 
//the assoicated resource upon leaving a scope (such as return, or exception). As there may be several exit points 
//even within some scope, manually free the source is very error prone.

namespace dnl
{
	namespace scoped_handle_priv
	{
		template<class F>
		struct close_func_traits;

		// function pointer
		template<class R, class T>
		struct close_func_traits<R(*)(T)> : public close_func_traits<R(T)>
		{};

#ifdef WIN32
		// stdcall calling convention
		template<class R, class T>
		struct close_func_traits<R(__stdcall)(T)> : public close_func_traits<R(T)>
		{};

		template<class R, class T>
		struct close_func_traits<R(__stdcall*)(T)> : public close_func_traits<R(T)>
		{};
#endif

		template<class R, class T>
		struct close_func_traits<R(T)>
		{
			using return_type = R;
			using handle_type = T;
		};
	}

	template<class close_func_t, 
		close_func_t close_func, 
		typename scoped_handle_priv::close_func_traits<close_func_t>::handle_type invalidHandle>
	class scoped_handle
	{
	public:
		using return_type = typename scoped_handle_priv::close_func_traits<close_func_t>::return_type;
		using handle_type = typename scoped_handle_priv::close_func_traits<close_func_t>::handle_type;

		scoped_handle() : m_handle(invalidHandle) {}
		explicit scoped_handle(const handle_type& handle) : m_handle(handle) {}

		~scoped_handle()
		{
			reset(invalidHandle);
		}


		return_type close()
		{
			return_type rc = close_func(m_handle);
			return rc;
		}

		void reset(handle_type handle = invalidHandle)
		{
			if ((m_handle != invalidHandle) && !!m_handle)
			{
				close();
			}

			m_handle = handle;
		}

		handle_type detach()
		{
			handle_type rsc = m_handle;
			m_handle = invalidHandle;
			return rsc;
		}

		scoped_handle& operator = (scoped_handle&& handle) { reset(handle.detach()); return *this; }
		operator handle_type () const { return m_handle; }
		explicit operator bool() const { return (invalidHandle != m_handle && !!m_handle); }
		bool operator == (const handle_type handle) const { return (handle == m_handle); }

	private:
		// Disable copy
		scoped_handle(const scoped_handle&);
		scoped_handle& operator = (const scoped_handle&);

		handle_type m_handle;
	};

} // namespace dnl
#endif //SCOPED_HANDLE_H


