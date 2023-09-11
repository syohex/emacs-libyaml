;;; test-libyaml.el --- Test for libyaml.el -*- lexical-binding: t -*-

;; Copyright (C) 2023 by Shohei YOSHIDA

;; Author: Shohei YOSHIDA <syohex@gmail.com>

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

(ert-deftest yaml-dump-vector ()
  "Vector to YAML string."
  (should (string= (yaml-dump '[1 2 3]) "---\n- 1\n- 2\n- 3\n..."))
  (should (string= (yaml-dump '["hello" 42]) "---\n- hello\n- 42\n..."))
  (should (string= (yaml-dump '[42 (56)]) "---\n- 42\n- - 56\n...")))

(ert-deftest yaml-dump-map ()
  "Vector to YAML string."
  (let ((m (make-hash-table :test #'equal)))
    (puthash "a" 1 m)
    (puthash "b" 2 m)
    (should (or (string= (yaml-dump m) "---\na: 1\nb: 2\n...")
                (string= (yaml-dump m) "---\nb: 2\na: 1\n...")))))

(ert-deftest yaml-dump-alist ()
  "Vector to YAML string."
  (let ((alist '((a . 1) (b . "foo"))))
    (should (or (string= (yaml-dump alist) "---\na: 1\nb: foo\n...")
                (string= (yaml-dump alist) "---\nb: foo\na: 1\n...")))))

;;; test-libyaml.el ends here
