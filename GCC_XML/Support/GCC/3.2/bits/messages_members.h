// std::messages implementation details, GNU version -*- C++ -*-

// Copyright (C) 2001, 2002, 2003 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

//
// ISO C++ 14882: 22.2.7.1.2  messages functions
//

// Written by Benjamin Kosnik <bkoz@redhat.com>

  // Non-virtual member functions.
  template<typename _CharT>
     messages<_CharT>::messages(size_t __refs)
     : locale::facet(__refs)
     {  
#if !(__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ > 2))
       _M_name_messages = _S_c_name;
#endif
       _M_c_locale_messages = _S_c_locale; 
     }

  template<typename _CharT>
     messages<_CharT>::messages(__c_locale __cloc, 
                                const char* __s, size_t __refs) 
     : locale::facet(__refs)
     {
#if !(__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ > 2)) 
       _M_name_messages = new char[strlen(__s) + 1];
       strcpy(_M_name_messages, __s);
#endif
       _M_c_locale_messages = _S_clone_c_locale(__cloc); 
     }

  template<typename _CharT>
    typename messages<_CharT>::catalog 
    messages<_CharT>::open(const basic_string<char>& __s, const locale& __loc, 
                           const char* __dir) const
    { 
      bindtextdomain(__s.c_str(), __dir);
      return this->do_open(__s, __loc); 
    }

  // Virtual member functions.
  template<typename _CharT>
    messages<_CharT>::~messages()
    { 
#if !(__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ > 2))
      if (_S_c_name != _M_name_messages)
        delete [] _M_name_messages;
#endif
      _S_destroy_c_locale(_M_c_locale_messages); 
    }

  template<typename _CharT>
    typename messages<_CharT>::catalog 
    messages<_CharT>::do_open(const basic_string<char>& __s, 
                              const locale&) const
    { 
      // No error checking is done, assume the catalog exists and can
      // be used.
      textdomain(__s.c_str());
      return 0;
    }

  template<typename _CharT>
    void    
    messages<_CharT>::do_close(catalog) const 
    { }

   // messages_byname
   template<typename _CharT>
     messages_byname<_CharT>::messages_byname(const char* __s, size_t __refs)
     : messages<_CharT>(__refs) 
     { 
#if !(__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ > 2))
       if (_S_c_name != _M_name_messages)
         delete [] _M_name_messages;
       _M_name_messages = new char[strlen(__s) + 1];
       strcpy(_M_name_messages, __s);
#endif
       this->_S_destroy_c_locale(this->_M_c_locale_messages);
       this->_S_create_c_locale(this->_M_c_locale_messages, __s); 
     }
