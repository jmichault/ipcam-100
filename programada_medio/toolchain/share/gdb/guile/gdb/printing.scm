;; Additional pretty-printer support.
;;
;; Copyright (C) 2014 Free Software Foundation, Inc.
;;
;; This file is part of GDB.
;;
;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 3 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

(define-module (gdb printing)
  #:use-module ((gdb) #:select
		(*pretty-printers* pretty-printer? objfile?
		 objfile-pretty-printers set-objfile-pretty-printers!))
  #:use-module (gdb init))

(define-public (prepend-pretty-printer! obj matcher)
  "Add MATCHER to the beginning of the pretty-printer list for OBJ.
If OBJ is #f, add MATCHER to the global list."
  (%assert-type (pretty-printer? matcher) matcher SCM_ARG1
		'prepend-pretty-printer!)
  (cond ((eq? obj #f)
	 (set! *pretty-printers* (cons matcher *pretty-printers*)))
	((objfile? obj)
	 (set-objfile-pretty-printers! obj
				       (cons matcher
					     (objfile-pretty-printers obj))))
	(else
	 (%assert-type #f obj SCM_ARG1 'prepend-pretty-printer!))))

(define-public (append-pretty-printer! obj matcher)
  "Add MATCHER to the end of the pretty-printer list for OBJ.
If OBJ is #f, add MATCHER to the global list."
  (%assert-type (pretty-printer? matcher) matcher SCM_ARG1
		'append-pretty-printer!)
  (cond ((eq? obj #f)
	 (set! *pretty-printers* (append! *pretty-printers* (list matcher))))
	((objfile? obj)
	 (set-objfile-pretty-printers! obj
				       (append! (objfile-pretty-printers obj)
						(list matcher))))
	(else
	 (%assert-type #f obj SCM_ARG1 'append-pretty-printer!))))
