#lang racket
(define (vector-add v1 v2)
  (if (null? v1)
      (list)
      (cons (+ (car v1) (car v2)) (vector-add (cdr v1) (cdr v2)))))

(define (norm2-square v)
  (if (null? v)
      0
      (+ (* (car v) (car v)) (norm2-square (cdr v)))))

(define (norm2 v)
  (sqrt (norm2-square v)))

(define (sign s)
  (if (> s 0) 1 -1))

(define (make-ek n m)
  (define (make-ek-iter n m k)
    (if (= k n)
        '()
        (cons (if (= m k) 1 0) (make-ek-iter n m (+ k 1)))))
  (make-ek-iter n m 0))

(define (make-identity-matrix n)
  (define (make-identity-matrix-iter n k)
    (if (= k n)
        '()
        (cons (make-ek n k) (make-identity-matrix-iter n (+ k 1)))))
  (make-identity-matrix-iter n 0))

(define (scalar-mat-mult s m)
  (if (null? m)
      '()
      (if (pair? m)
          (cons (scalar-mat-mult s (car m)) (scalar-mat-mult s (cdr m)))
          (* s m))))

(define (vector-mult v1 v2)
  (if (null? v1)
      0
      (+ (* (car v1) (car v2)) (vector-mult (cdr v1) (cdr v2)))))

(define (transpose-matrix m)
  (cons (map (lambda (w) (car w)) m) 
        (if (null? (cdr (car m))) 
            '() 
            (transpose-matrix (map (lambda (w) (cdr w)) m)))))

(define (mat-mat-mult m1 m2)
  (map (lambda (v) 
         (map 
          (lambda (w) 
            (vector-mult v w)) (transpose-matrix m2))) m1))

(define (mat-mat-add m1 m2)
  (if (null? m1)
      '()
      (if (pair? m1)
          (cons (mat-mat-add (car m1) (car m2)) (mat-mat-add (cdr m1) (cdr m2)))
          (+ m1 m2))))

(define (vector-len v)
  (if (null? v)
      0
      (+ 1 (vector-len (cdr v)))))

(define (reverse-vec v)
  (define (reverse-vec-iter r v)
    (if (null? v)
        r
        (reverse-vec-iter (cons (car v) r) (cdr v))))
  (reverse-vec-iter '() v))

(define (householder v)
  (if (< (norm2-square v) 1.0e-12)
      (make-identity-matrix (vector-len v))
      (mat-mat-add
       (make-identity-matrix (vector-len v))
       (let [(u (mat-mat-add v (scalar-mat-mult (norm2 v) (make-ek (vector-len v) 0))))]
         (scalar-mat-mult 
          (/ -2. (norm2-square u))
          (mat-mat-mult (transpose-matrix (list u)) (list u)))))))

;(mat-mat-mult (householder '(1 0 1)) (transpose-matrix (list '(1 0 1))))

(define (range-list n)
  (if (= n 0)
      '()
      (append (range-list (- n 1)) (list n))))

(define (zero-list n)
  (if (= n 0)
      '()
      (cons 0 (zero-list (- n 1)))))

(define (extend-matrix m k)
  (if (= k 0)
      m
      (let [(len (+ (vector-len (car m)) k))]
        (append 
         (map (lambda (i) (make-ek len (- i 1))) (range-list k)) 
         (map (lambda (v) (append (zero-list k) v)) m)))))

(define (trim-vector v k)
    (if (= k 0)
        v
        (trim-vector (cdr v) (- k 1))))

(define (qr-decompose m)
  (define hlen (vector-len (car m)))
  (define vlen (vector-len m))
  (define (pick-vector m k)
    (if (= k 0)
        (car m)
        (pick-vector (cdr m) (- k 1))))
  (define (qr-decompose-iter q r k)
    (if (= k hlen)
        (cons q r)
        (let [(h (extend-matrix 
                  (householder 
                   (trim-vector 
                    (pick-vector (transpose-matrix r) k) k)) k))]
          (qr-decompose-iter 
           (mat-mat-mult q (transpose-matrix h)) 
           (mat-mat-mult h r) 
           (+ k 1)))))
  (qr-decompose-iter (make-identity-matrix vlen) m 0))

;(range-list 10)
;(extend-matrix (list '(1 0) '(2 1)) 3)
(define res (qr-decompose (list '(1 2) '(3 4) '(5 6))))
(display "\n---------\n")
(display (car res))
(display "\n---------\n")
(display (cdr res))
(display "\n---------\n")
(mat-mat-mult (transpose-matrix (car res)) (list '(7) '(8) '(9)))
(define (solve-linear-regression param val)
  (define res (qr-decompose param))
  (define qmat (cdr res))
  (define upper-mat (cdr res))
  (define new-val (mat-mat-mult (transpose-matrix qmat) val))
  (define vlen (vector-len upper-mat))
  (define hlen (vector-len upper-mat))
  (define (solve-upper-iter rupper-mat rval rres k)
   (if (= k hlen)
    (reverse-vec rres)
    (solve-upper-iter (cdr rupper-mat) (cdr rval) () (+ k 1)))
  ;reverse and trim unwanted elements
  (let* ([rnew-val (trim-vector (reverse-vec new-val) (- vlen hlen))] 
         [rupper-mat (trim-vector (reverse-vec upper-mat) (-vlen hlen))]
         
         )
    


