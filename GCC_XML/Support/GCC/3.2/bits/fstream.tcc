// File based streams -*- C++ -*-

// Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002, 2003
// Free Software Foundation, Inc.
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
// ISO C++ 14882: 27.8  File-based streams
//

#ifndef _CPP_BITS_FSTREAM_TCC
#define _CPP_BITS_FSTREAM_TCC 1

#pragma GCC system_header

namespace std
{
  template<typename _CharT, typename _Traits>
    void
    basic_filebuf<_CharT, _Traits>::
    _M_allocate_internal_buffer()
    {
      if (!this->_M_buf && this->_M_buf_size_opt)
        {
          this->_M_buf_size = this->_M_buf_size_opt;

          // Allocate internal buffer.
          this->_M_buf = new char_type[this->_M_buf_size]; 
          this->_M_buf_allocated = true;
        }
    }

  // Both close and setbuf need to deallocate internal buffers, if it exists.
  template<typename _CharT, typename _Traits>
    void
    basic_filebuf<_CharT, _Traits>::
    _M_destroy_internal_buffer()
    {
      if (this->_M_buf_allocated)
        {
          delete [] this->_M_buf;
          this->_M_buf = NULL;
          this->_M_buf_allocated = false;
          this->setg(NULL, NULL, NULL);
          this->setp(NULL, NULL);
        }
    }

  template<typename _CharT, typename _Traits>
    basic_filebuf<_CharT, _Traits>::
    basic_filebuf() : __streambuf_type(), _M_file(&_M_lock), 
    _M_state_cur(__state_type()), _M_state_beg(__state_type()), 
    _M_buf_allocated(false), _M_last_overflowed(false)
    { this->_M_buf_unified = true; }

  template<typename _CharT, typename _Traits>
    typename basic_filebuf<_CharT, _Traits>::__filebuf_type* 
    basic_filebuf<_CharT, _Traits>::
    open(const char* __s, ios_base::openmode __mode)
    {
      __filebuf_type *__ret = NULL;
      if (!this->is_open())
        {
          _M_file.open(__s, __mode);
          if (this->is_open())
            {
              _M_allocate_internal_buffer();
              this->_M_mode = __mode;
              _M_set_indeterminate();

              if ((__mode & ios_base::ate)
                  && this->seekoff(0, ios_base::end, __mode) < 0)
                {
                  // 27.8.1.3,4
                  this->close();
                  return __ret;
                }

              __ret = this;
            }
        }
      return __ret;
    }

  template<typename _CharT, typename _Traits>
    typename basic_filebuf<_CharT, _Traits>::__filebuf_type* 
    basic_filebuf<_CharT, _Traits>::
    close()
    {
      __filebuf_type *__ret = NULL;
      if (this->is_open())
        {
          const int_type __eof = traits_type::eof();
          bool __testput = this->_M_out_cur && this->_M_out_beg < this->_M_out_end;
          if (__testput 
              && traits_type::eq_int_type(_M_really_overflow(__eof), __eof))
            return __ret;

          // NB: Do this here so that re-opened filebufs will be cool...
          this->_M_mode = ios_base::openmode(0);
          this->_M_destroy_internal_buffer();
          this->_M_pback_destroy();
          
#if 0
          // XXX not done
          if (_M_last_overflowed)
            {
              _M_output_unshift();
              _M_really_overflow(__eof);
            }
#endif

          if (_M_file.close())
            __ret = this;
        }

      _M_last_overflowed = false;       
      return __ret;
    }

  template<typename _CharT, typename _Traits>
    streamsize 
    basic_filebuf<_CharT, _Traits>::
    showmanyc()
    {
      streamsize __ret = -1;
      bool __testin = this->_M_mode & ios_base::in;

      if (__testin && this->is_open())
        __ret = this->_M_in_end - this->_M_in_cur;
      this->_M_last_overflowed = false; 
      return __ret;
    }
  
  template<typename _CharT, typename _Traits>
    typename basic_filebuf<_CharT, _Traits>::int_type 
    basic_filebuf<_CharT, _Traits>::
    pbackfail(int_type __i)
    {
      int_type __ret = traits_type::eof();
      bool __testin = this->_M_mode & ios_base::in;

      if (__testin)
        {
          bool __testpb = this->_M_in_beg < this->_M_in_cur;
          char_type __c = traits_type::to_char_type(__i);
          bool __testeof = traits_type::eq_int_type(__i, __ret);

          if (__testpb)
            {
              bool __testout = this->_M_mode & ios_base::out;
              bool __testeq = traits_type::eq(__c, this->gptr()[-1]);

              // Try to put back __c into input sequence in one of three ways.
              // Order these tests done in is unspecified by the standard.
              if (!__testeof && __testeq)
                {
                  --this->_M_in_cur;
                  if (__testout)
                    --this->_M_out_cur;
                  __ret = __i;
                }
              else if (__testeof)
                {
                  --this->_M_in_cur;
                  if (__testout)
                    --this->_M_out_cur;
                  __ret = traits_type::not_eof(__i);
                }
              else if (!__testeof)
                {
                  --this->_M_in_cur;
                  if (__testout)
                    --this->_M_out_cur;
                  this->_M_pback_create();
                  *this->_M_in_cur = __c; 
                  __ret = __i;
                }
            }
          else
            {    
              // At the beginning of the buffer, need to make a
              // putback position available.
              this->seekoff(-1, ios_base::cur);
              this->underflow();
              if (!__testeof)
                {
                  if (!traits_type::eq(__c, *this->_M_in_cur))
                    {
                      this->_M_pback_create();
                      *this->_M_in_cur = __c;
                    }
                  __ret = __i;
                }
              else
                __ret = traits_type::not_eof(__i);
            }
        }
      this->_M_last_overflowed = false; 
      return __ret;
    }

  template<typename _CharT, typename _Traits>
    typename basic_filebuf<_CharT, _Traits>::int_type 
    basic_filebuf<_CharT, _Traits>::
    overflow(int_type __c)
    {
      int_type __ret = traits_type::eof();
      bool __testput = this->_M_out_cur && this->_M_out_cur < this->_M_buf + this->_M_buf_size;
      bool __testout = this->_M_mode & ios_base::out;
      
      if (__testout)
        {
          if (__testput)
            {
              *this->_M_out_cur = traits_type::to_char_type(__c);
              this->_M_out_cur_move(1);
              __ret = traits_type::not_eof(__c);
            }
          else 
            __ret = this->_M_really_overflow(__c);
        }

      this->_M_last_overflowed = false;    // Set in this->_M_really_overflow, below.
      return __ret;
    }
  
  template<typename _CharT, typename _Traits>
    void
    basic_filebuf<_CharT, _Traits>::
    _M_convert_to_external(_CharT* __ibuf, streamsize __ilen,
                           streamsize& __elen, streamsize& __plen)
    {
      const locale __loc = this->getloc();
      const __codecvt_type& __cvt = use_facet<__codecvt_type>(__loc);
      
      if (__cvt.always_noconv() && __ilen)
        {
          __elen += _M_file.xsputn(reinterpret_cast<char*>(__ibuf), __ilen);
          __plen += __ilen;
        }
      else
        {
          // Worst-case number of external bytes needed.
          int __ext_multiplier = __cvt.encoding();
          if (__ext_multiplier ==  -1 || __ext_multiplier == 0)
            __ext_multiplier = sizeof(char_type);
          streamsize __blen = __ilen * __ext_multiplier;
          char* __buf = static_cast<char*>(__builtin_alloca(__blen));
          char* __bend;
          const char_type* __iend;
          __res_type __r = __cvt.out(_M_state_cur, __ibuf, __ibuf + __ilen, 
                                     __iend, __buf, __buf + __blen, __bend);

          if (__r == codecvt_base::ok || __r == codecvt_base::partial)
            __blen = __bend - __buf;
          // Similarly to the always_noconv case above.
          else if (__r == codecvt_base::noconv)
            {
              __buf = reinterpret_cast<char*>(__ibuf);
              __blen = __ilen;
            }
          // Result == error
          else 
            __blen = 0;
          
          if (__blen)
            {
              __elen += _M_file.xsputn(__buf, __blen);
              __plen += __blen;
            }

          // Try once more for partial conversions.
          if (__r == codecvt_base::partial)
            {
              const char_type* __iresume = __iend;
              streamsize __rlen = this->_M_out_end - __iend;
              __r = __cvt.out(this->_M_state_cur, __iresume, __iresume + __rlen, 
                              __iend, __buf, __buf + __blen, __bend);
              if (__r != codecvt_base::error)
                __rlen = __bend - __buf;
              else 
                __rlen = 0;
              if (__rlen)
                {
                  __elen += _M_file.xsputn(__buf, __rlen);
                  __plen += __rlen;
                }
            }
        }
    }

  template<typename _CharT, typename _Traits>
    typename basic_filebuf<_CharT, _Traits>::int_type 
    basic_filebuf<_CharT, _Traits>::
    _M_really_overflow(int_type __c)
    {
      int_type __ret = traits_type::eof();
      bool __testput = this->_M_out_cur && this->_M_out_beg < this->_M_out_end;
      bool __testunbuffered = this->_M_file.is_open() && !this->_M_buf_size_opt;

      if (__testput || __testunbuffered)
        {
          // Sizes of external and pending output.
          streamsize __elen = 0;
          streamsize __plen = 0;

          // Need to restore current position. The position of the external
          // byte sequence (_M_file) corresponds to _M_filepos, and we need
          // to move it to _M_out_beg for the write.
          if (this->_M_filepos && this->_M_filepos != this->_M_out_beg)
            {
              off_type __off = this->_M_out_beg - this->_M_filepos;
              this->_M_file.seekoff(__off, ios_base::cur);
            }

          // Convert internal buffer to external representation, output.
          // NB: In the unbuffered case, no internal buffer exists. 
          if (!__testunbuffered)
            _M_convert_to_external(this->_M_out_beg,  this->_M_out_end - this->_M_out_beg, 
                                   __elen, __plen);

          // Convert pending sequence to external representation, output.
          // If eof, then just attempt sync.
          if (!traits_type::eq_int_type(__c, traits_type::eof()))
            {
              char_type __pending = traits_type::to_char_type(__c);
              _M_convert_to_external(&__pending, 1, __elen, __plen);

              // User code must flush when switching modes (thus don't sync).
              if (__elen == __plen)
                {
                  _M_set_indeterminate();
                  __ret = traits_type::not_eof(__c);
                }
            }
          else if (!_M_file.sync())
            {
              _M_set_indeterminate();
              __ret = traits_type::not_eof(__c);
            }
        }             
      _M_last_overflowed = true;        
      return __ret;
    }

  template<typename _CharT, typename _Traits>
    typename basic_filebuf<_CharT, _Traits>::__streambuf_type* 
    basic_filebuf<_CharT, _Traits>::
    setbuf(char_type* __s, streamsize __n)
    {
      if (!this->is_open() && __s == 0 && __n == 0)
        this->_M_buf_size_opt = 0;
      else if (__s && __n)
        {
          // This is implementation-defined behavior, and assumes
          // that an external char_type array of length (__s + __n)
          // exists and has been pre-allocated. If this is not the
          // case, things will quickly blow up.
          // Step 1: Destroy the current internal array.
          _M_destroy_internal_buffer();
          
          // Step 2: Use the external array.
          this->_M_buf = __s;
          this->_M_buf_size_opt = this->_M_buf_size = __n;
          _M_set_indeterminate();
        }
      _M_last_overflowed = false;       
      return this; 
    }
  
  template<typename _CharT, typename _Traits>
    typename basic_filebuf<_CharT, _Traits>::pos_type
    basic_filebuf<_CharT, _Traits>::
    seekoff(off_type __off, ios_base::seekdir __way, ios_base::openmode __mode)
    {
      pos_type __ret =  pos_type(off_type(-1)); 
      bool __testin = (ios_base::in & this->_M_mode & __mode) != 0;
      bool __testout = (ios_base::out & this->_M_mode & __mode) != 0;

      // Should probably do has_facet checks here.
      int __width = use_facet<__codecvt_type>(this->_M_buf_locale).encoding();
      if (__width < 0)
        __width = 0;
      bool __testfail = __off != 0 && __width <= 0;
      
      if (this->is_open() && !__testfail && (__testin || __testout)) 
        {
          // Ditch any pback buffers to avoid confusion.
          this->_M_pback_destroy();

          if (__way != ios_base::cur || __off != 0)
            { 
              off_type __computed_off = __width * __off;
              
              bool __testget = this->_M_in_cur && this->_M_in_beg < this->_M_in_end;
              bool __testput = this->_M_out_cur && this->_M_out_beg < this->_M_out_end;
              // Sync the internal and external streams.
              // out
              if (__testput || this->_M_last_overflowed)
                {
                  // Part one: update the output sequence.
                  this->sync();
                  // Part two: output unshift sequence.
                  this->_M_output_unshift();
                }
              //in
              else if (__testget && __way == ios_base::cur)
                __computed_off += this->_M_in_cur - this->_M_filepos;
          
              __ret = this->_M_file.seekoff(__computed_off, __way, __mode);
              this->_M_set_indeterminate();
            }
          // NB: Need to do this in case this->_M_file in indeterminate
          // state, ie _M_file._offset == -1
          else
            {
              __ret = _M_file.seekoff(__off, ios_base::cur, __mode);
              __ret += max(this->_M_out_cur, this->_M_in_cur) - this->_M_filepos;
            }
        }
      this->_M_last_overflowed = false; 
      return __ret;
    }

  template<typename _CharT, typename _Traits>
    typename basic_filebuf<_CharT, _Traits>::pos_type
    basic_filebuf<_CharT, _Traits>::
    seekpos(pos_type __pos, ios_base::openmode __mode)
    {
#ifdef _GLIBCPP_RESOLVE_LIB_DEFECTS
// 171. Strange seekpos() semantics due to joint position
      return this->seekoff(off_type(__pos), ios_base::beg, __mode);
#endif
    }

  template<typename _CharT, typename _Traits>
    void 
    basic_filebuf<_CharT, _Traits>::
    _M_output_unshift()
    { }

  template<typename _CharT, typename _Traits>
    void
    basic_filebuf<_CharT, _Traits>::
    imbue(const locale& __loc)
    {
      bool __testbeg = this->gptr() == this->eback() && this->pptr() == this->pbase();

      if (__testbeg && this->_M_buf_locale != __loc)
        this->_M_buf_locale = __loc;

      // NB this may require the reconversion of previously
      // converted chars. This in turn may cause the reconstruction
      // of the original file. YIKES!!
      // XXX The part in the above comment is not done.
      this->_M_last_overflowed = false; 
    }

  // Inhibit implicit instantiations for required instantiations,
  // which are defined via explicit instantiations elsewhere.  
  // NB:  This syntax is a GNU extension.
  extern template class basic_filebuf<char>;
  extern template class basic_ifstream<char>;
  extern template class basic_ofstream<char>;
  extern template class basic_fstream<char>;

#ifdef _GLIBCPP_USE_WCHAR_T
  extern template class basic_filebuf<wchar_t>;
  extern template class basic_ifstream<wchar_t>;
  extern template class basic_ofstream<wchar_t>;
  extern template class basic_fstream<wchar_t>;
#endif
} // namespace std

#endif 
