;(setq exec-path (cons "/usr/local/bin" exec-path))
(setenv "PATH" (concat "/usr/local/bin" ":" (getenv "PATH")))

(require 'package)
(add-to-list 'package-archives
	     '("gnu" . "https://elpa.gnu.org/packages/") t)
(add-to-list 'package-archives
	     '("melpa" . "https://melpa.org/packages/") t)

;; Added by Package.el.  This must come before configurations of
;; installed packages.  Don't delete this line.  If you don't want it,
;; just comment it out by adding a semicolon to the start of the line.
;; You may delete these explanatory comments.
(package-initialize)

(setq inhibit-startup-screen t)
(setq use-package-always-ensure nil)

(unless (require 'use-package nil t)
  (if (not (yes-or-no-p (concat "Refresh packages, install use-package and"
                                " other packages used by init file? ")))
      (error "you need to install use-package first")
    (package-refresh-contents)
    (package-install 'use-package)
    (require 'use-package)
    (setq use-package-always-ensure t)))

(use-package which-key
  :config
  (which-key-mode))

(global-set-key (kbd "C-c h b") 'describe-personal-keybindings)

(setq recentf-max-saved-items 100)

(global-set-key "\C-cq" #'bury-buffer)

(use-package helm
  :demand
  :init
  (setq helm-split-window-default-side 'other
        helm-split-window-inside-p t
        helm-swoop-split-with-multiple-windows t	
        helm-command-prefix-key "s-c")
  :config
  (require 'helm-config)	      ; required to setup "s-c" keymap
  (helm-mode 1)
  (helm-autoresize-mode 1)
  ;; Only rebind M-x and C-x C-f on successful load of helm to remain
  ;; this basic operations if helm is not installed.
  (bind-key "M-x" #'helm-M-x)
  (bind-key "C-x C-f" #'helm-find-files)
  :bind
  (("M-y" . helm-show-kill-ring)
   ("C-c o" . helm-occur)
   ("C-x b" . helm-mini)
   ("C-x r b" . helm-bookmarks)
   ("C-h a" . helm-apropos)
   ("C-h d" . helm-info-at-point)
   ("C-c a" . helm-all-mark-rings)
   ("C-c h e" . helm-info-emacs)
   ("C-c h g" . helm-info-gnus)
   ("C-c R" . helm-register)
   ("s-P" . helm-run-external-command)
   ;; More key bindings in "s-c" keymap
   :map helm-find-files-map
   ("<backtab>" . helm-select-action)
   ("C-i" . helm-execute-persistent-action)))

(use-package ace-window
  :init
  (setq aw-scope 'frame ; limit to single frame (useful when using exwm)
        aw-keys '(?a ?s ?d ?f ?g ?h ?j ?k ?l))
  :bind
  ("C-x o" . ace-window))

(use-package helm-swoop
  :bind
  (("C-s" . helm-swoop-without-pre-input)
   ("C-S-s" . helm-swoop)))

(use-package magit
  :bind (("C-x g" . magit-status)
         ("C-x M-g" . magit-dispatch)))

(use-package git-commit
  :hook (git-commit-mode . my-american-dict))

(use-package git-messenger
  :bind ("C-x G" . git-messenger:popup-message)
  :config
  (setq git-messenger:show-detail t
        git-messenger:use-magit-popup t))

(use-package helm-git-grep
  :bind
  (("C-c j" . helm-git-grep)
   ("C-c J" . helm-git-grep-at-point)))

(use-package helm-ls-git
  :bind
  (("C-c g" . helm-ls-git-ls)))

(defun export-to-jekyll-html ()
  (interactive)
  (let* ((fname (buffer-file-name))
         (dname (file-name-directory fname))
         (base-name (file-name-base fname)))
    (if (not (string= (file-name-extension fname) "org"))
      (message "not a org-mode file, skip ...")
      (progn
        (org-html-export-as-html)
        (switch-to-buffer "*Org HTML Export*")
        (goto-char (point-min))
        (search-forward "<div id=\"content\" class=\"content\">")
        (kill-region (point-min) (+ 1 (point)))
        (goto-char (point-max))
        (search-backward "</div>")
        (kill-region (point) (point-max))
        (write-file (format "%s../_posts/%s.html" dname base-name))
        (kill-buffer (format "%s.html" base-name))
        (delete-window)))))

(show-paren-mode 1)
(setq-default abbrev-mode 1)
(setq-default indent-tabs-mode nil)
(setq-default tab-width 4)

(use-package yasnippet
  :hook (after-init . yas-global-mode)
  :bind
  (:map yas-minor-mode-map
        ("C-c & t" . yas-describe-tables)
        ("C-c & &" . org-mark-ring-goto)))

(use-package yasnippet-snippets
  :defer)

(use-package helm-c-yasnippet
  :bind
  (("C-c y" . helm-yas-complete)))

(add-hook 'after-init-hook 'global-company-mode)

(use-package org
  :init
  (setq org-export-with-toc nil)
  (setq org-publish-project-alist
        '(("org-paul356"
           ;; Path to your org files.
           :base-directory "~/code/paul356.github.io/org/"
           :base-extension "org"
           ;; Path to your Jekyll project.
           :publishing-directory "~/code/paul356.github.io/_posts/"
           :recursive t
           :publishing-function org-html-publish-to-html
           :headline-levels 4
           :html-extension "html"
           :body-only t ;; Only export section between <body> </body>
           )
          ("org-paul356-static"
           :base-directory "~/code/paul356.github.io/org/"
           :base-extension "css\\|js\\|png\\|jpg\\|gif\\|pdf\\|mp3\\|ogg\\|swf\\|php"
           :publishing-directory "~/code/paul356.github.io/images/"
           :recursive t
           :publishing-function org-publish-attachment)
          ("blog" :components ("org-paul356" "org-paul356-static")))))

(defconst my-cc-style
  '("k&r"
    (c-offsets-alist . ((innamespace . [0])))))

(c-add-style "my-cc-mode" my-cc-style)

(setq-default c-basic-offset 4)
(setq c-default-style '((java-mode . "java")
                        (awk-mode . "awk")
                        (c++-mode . "my-cc-mode")
                        (other . "k&r")))

(add-to-list 'auto-mode-alist '("\\.ino\\'" . c++-mode))

(add-hook 'c-mode-common-hook 'cscope-setup)

;; (use-package cmake-ide
;;   :after cc-mode
;;   :config
;;   (cmake-ide-setup))

(custom-set-variables
 ;; custom-set-variables was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(cscope-option-do-not-update-database t)
 '(cscope-program "/usr/local/bin/cscope")
 '(org-mode-hook
   (quote
    (#[0 "\300\301\302\303\304$\207"
         [add-hook change-major-mode-hook org-show-all append local]
         5]
     #[0 "\300\301\302\303\304$\207"
         [add-hook change-major-mode-hook org-babel-show-result-all append local]
         5]
     org-babel-result-hide-spec org-babel-hide-all-hashes valign-mode)))
 '(package-selected-packages
   (quote
    (valign scala-mode org s xcscope git-messenger helm-git-grep helm-ls-git magit helm-c-yasnippet ace-window helm helm-swoop yasnippet-snippets which-key cmake-ide zygospore ws-butler volatile-highlights use-package undo-tree rtags-xref projectile iedit flycheck dtrt-indent company comment-dwim-2 clojure-snippets clean-aindent-mode anzu))))
(custom-set-faces
 ;; custom-set-faces was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 )
