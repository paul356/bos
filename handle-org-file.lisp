(defun starts-with? (str substr)
  (let ((sublen (length substr)))
    (and (> (length str) sublen) (string= (subseq str 0 sublen) substr))))

(defun space-char? (ch)
  (or (char= ch #\Space) (char= ch #\Tab)))

(defun empty-line? (line)
  (reduce #'(lambda (&rest vars) (if (eq (length vars) 2) (and (first vars) (second vars)) t)) (map 'list #'(lambda (x) (space-char? x)) line)))

(defun print-head (fout)
  (format fout "~a~%" "|-----------------------------------------------------------------------------------------+-------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+--------------+---------------------------------------|")
  (format fout "~a~%" "| Title                                                                                   | Authors                                                     | Synthesis                                                                                                                                                                                   | Publisher    | Keywords                              |")
  (format fout "~a~%" "|-----------------------------------------------------------------------------------------+-------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+--------------+---------------------------------------|"))

(defun print-tail (fout)
  (format fout "~a~%" "|-----------------------------------------------------------------------------------------+-------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+--------------+---------------------------------------|"))

(defun handle-one-org-file (orgfile)
  (let ((new-file (make-pathname :directory (pathname-directory orgfile) :name (pathname-name orgfile) :type "txt")))
    (with-open-file (fs orgfile :direction :input)
      (with-open-file (fout
                       new-file
                       :direction :output
                       :if-exists :supersede)
        (let ((records (list '(Title . "") '(Author . "") '(Publisher . "") '(Keywords . "") '(Abstract . "")))
              (skip nil)
              (headstr nil))
          (do ((line (read-line fs nil 'eof) (read-line fs nil 'eof)))
              ((eq line 'eof) nil)
            (cond
              ((starts-with? line "#+BEGIN") (setq skip t) (format fout "~a~%" line))
              ((starts-with? line "#+END") (setq skip nil) (format fout "~a~%" line))
              ((or (starts-with? line "#+") skip) (format fout "~a~%" line))
              ((empty-line? line) nil)
              ((starts-with? line "** ") (rplacd (assoc 'Title records) (string-trim " \\" (subseq line 3))))
              ((starts-with? line "Author:") (rplacd (assoc 'Author records) (string-trim " \\" (subseq line (length "Author:")))))
              ((starts-with? line "Publisher:") (rplacd (assoc 'Publisher records) (string-trim " \\" (subseq line (length "Publisher:")))))
              ((starts-with? line "Keywords:") (rplacd (assoc 'Keywords records) (string-trim " \\" (subseq line (length "Keywords:")))))
              (t (progn (rplacd (assoc 'Abstract records) line)
                        (unless headstr
                          (print-head fout)
                          (setq headstr t))
                        (format fout "| ~a | ~a | ~a | ~a | ~a |~%"
                                (cdr (assoc 'Title records))
                                (cdr (assoc 'Author records))
                                (cdr (assoc 'Abstract records))
                                (cdr (assoc 'Publisher records))
                                (cdr (assoc 'Keywords records))))))))
        (print-tail fout)))
    (rename-file new-file orgfile)))

(handle-one-org-file #P"/home/user1/code/paul356.github.io/_org/2023-06-26-june-papers.org")
(handle-one-org-file #P"/home/user1/code/paul356.github.io/_org/2023-07-18-july-papers.org")
(handle-one-org-file #P"/home/user1/code/paul356.github.io/_org/2023-09-03-september-papers.org")
