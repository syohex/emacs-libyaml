;;; test-libyaml.el --- Test for libyaml.el

;; Copyright (C) 2016 by Syohei YOSHIDA

;; Author: Syohei YOSHIDA <syohex@gmail.com>

;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.

;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

;;; Commentary:

;;; Code:

(require 'ert)
(require 'libyaml)
(require 'cl-lib)
(require 'subr-x)

(ert-deftest read-from-string ()
  "Read YAML from string"
  (let ((got (yaml-read-from-string "
language: generic
sudo: false
before_install:
  - echo foobar
  - ls
script:
  - make test
")))
    (should (cl-typep got 'hash-table))
    (should (= (length (hash-table-keys got)) 4))
    (should (string= (gethash "language" got) "generic"))
    (should (cl-typep (gethash "before_install" got) 'vector))
    (should (string= (aref (gethash "script" got) 0) "make test"))))

;;; test-libyaml.el ends here
