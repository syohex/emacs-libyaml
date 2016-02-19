;;; libyaml.el --- libyaml binding

;; Copyright (C) 2016 by Syohei YOSHIDA

;; Author: Syohei YOSHIDA <syohex@gmail.com>
;; URL: https://github.com/syohex/emacs-libyaml
;; Version: 0.01

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

(require 'libyaml-core)
(require 'subr-x)

;;;###autoload
(defun yaml-read-from-string (str)
  (libyaml-load str))

;;;###autoload
(defun yaml-read-file (file)
  (interactive
   (list (read-file-name "YAML File: ")))
  (yaml-read-from-string
   (with-current-buffer (find-file-noselect file)
     (buffer-substring-no-properties (point-min) (point-max)))))

;;;###autoload
(defun yaml-dump (obj)
  (libyaml-dump obj))

(provide 'libyaml)

;;; libyaml.el ends here
