#lang racket

(define-namespace-anchor anc)
(define ns (namespace-anchor->namespace anc))
;; given
(struct num (value grad)
    #:property prop:custom-write
    (lambda (num port write?)
        (fprintf port (if write? "(num ~s ~s)" "(num ~a ~a)")
            (num-value num) (num-grad num))))

;3.1
(define (get-value-of-num num) (car(cdr num)))
(define (get-value num-list) (if (list? num-list) (map get-value-of-num num-list) (num-value num-list)))
(get-value (num 10 2))
(get-value '((num 3 1) (num 4 2) (num 0.0001 0.0)))

;3.2
(define (get-grad-of-num num) (car(cdr (cdr num))))
(define (get-grad num-list) (if (list? num-list) (map get-grad-of-num num-list) (num-grad num-list)))
(get-grad '((num 3 1) (num 4 2) (num 0.0001 0.0)))
(get-grad (num 10 2))

;4.1
(define getv (lambda list (map (lambda (x) (num-value x)) list)))
(define getg (lambda list (map (lambda (x) (num-grad x)) list)))
(define add (lambda list (num (apply + (apply getv list)) (apply + (apply getg list)))))
(add (num 5 1) (num 2 0.2))
(add (num 5 1) (num 4 3) (num 6 2))

;4.2
;(define mul (lambda list (num (apply * (apply getv list)) (+ (* (num-value (car list)) (num-grad (car(cdr list))) ) (* (num-grad (car list)) (num-value (car(cdr list)))) ))))
(define mul (lambda lis (num (apply * (apply getv lis)) (mulg lis (- (length lis) 1)))))
(define (mulg list index)
  (if (< index 0) 0 (+ (/(apply * (num-grad (list-ref list index)) (apply getv list) ) (num-value (list-ref list index))) (mulg list (- index 1)) ) )
  )
(mul (num 5 1) (num 2 1))
(mul (num 5 1) (num 4 3) (num 6 2)) 
(mul (num 5 1) (num 4 3) (num 6 2) (num 5 10)) 

;4.3
(define sub (lambda (num1 num2) (num (- (num-value num1) (num-value num2)) (- (num-grad num1) (num-grad num2)) )))
(sub (num 5 1) (num 2 1))
(sub (num 5 0) (num 2 1))

;; given
(define relu (lambda (x) (if (> (num-value x) 0) x (num 0.0 0.0))))
;; given
(define mse (lambda (x y) (mul (sub x y) (sub x y))))

;5.1
(define (create-var-list var length) ( if (= length 0) '() (cons var (create-var-list var (- length 1))) ) )
(define (create-hash names values var)
  ;(apply hash(combine names (map hass names values (create-var-list var (length names)))))
  (make-hash (map hass names values (create-var-list var (length names))))
)
;(define (hass name value var) (if (equal? name var)  (num value 1.0) (num value 0.0)))
(define (hass name value var) (if (equal? name var)  (cons name (num value 1.0)) (cons name (num value 0.0))))
(define (combine list1 list2) (if (= 0 (length list1)) '() (append (list(car list1)(car list2)) (combine (cdr list1) (cdr list2) ))))
(create-hash '(a b c d) '(1 3 3 7) 'b)
(create-hash '(a b c d) '(1 3 3 7) 'c)
(create-hash '(a b c d e f g h) '(1 3 3.1 7.9 4 5 8 9) 'e)
(create-hash '(x y) '(10 20) 'x)

;(hash-ref '#hash((a . (num 1 0.0)) (b . (num 3 1.0)) (c . (num 3 0.0)) (d . (num 7 0.0)))  'c)

;5.2
(define (parse hash expr)
  ( cond ( (null? expr) '() )
         ( (list? expr) (cons (parse hash (car expr)) (parse hash (cdr expr)) ) )
         ( (number? expr) (num expr 0.0) )
         ( (equal? expr '+) 'add )
         ( (equal? expr '-) 'sub )
         ( (equal? expr '*) 'mul )
         ( (equal? expr 'mse) 'mse )
         ( (equal? expr 'relu) 'relu )
         (else (hash-ref hash expr) )
         )
  )

(parse '#hash((x . (num 10 1.0)) (y . (num 20 0.0))) '(+ x y))
(parse (create-hash '(x y) '(10 20) 'x) '(+ x y))
(eval (parse '#hash((x . (num 10 1.0)) (y . (num 20 0.0))) '(+ x y)) ns)
(parse (create-hash '(x y) '(10 20) 'x) '(+ (* (+ x y) x) (+ y x 5)))

;5.3
(define (grad names values var expr) (num-grad (eval (parse (create-hash names values var) expr ) ns) ) )
(grad '(x) '(7) 'x '(* x x))
(grad '(x y) '(10 20) 'x '(+ (* (+ x y) x) (+ y x)))

;5.4
(define (partial-grad names values vars expr) (map (lambda (name) (if (member name vars) (grad names values name expr) 0.0 )) names) )
(partial-grad '(x y) '(10 20) '(x y) '(+ 1 2))
(partial-grad '(x y) '(10 20) '(x) '(+ (* (+ x y) x) (+ y x)))
(partial-grad '(x y) '(10 20) '(y) '(+ (* (+ x y) x) (+ y x)))
(partial-grad '(x y) '(10 20) '(x y) '(+ (* (+ x y) x) (+ y x)))

;5.5
(define (gradient-descent names values vars lr expr) (map - values (map (lambda (n) (* lr n)) (partial-grad names values vars expr))))
;(map (lambda (n) (* 0.1 n)) (partial-grad '(x y) '(10 20) '(x) '(+ x y)))

(gradient-descent '(x y) '(10 20) '(x) 0.1 '(+ x y))
(gradient-descent '(x y) '(10 20) '(x y) 0.1 '(+ x y))
(gradient-descent '(x y) '(10 20) '(x y) 0.001 '(+ x y))
(gradient-descent '(x y) '(10 20) '(x) 0.1 '(mse x y))

;5.6
(define (optimize names values vars lr k expr) (if (= k 1)
                                                   (gradient-descent names values vars lr expr)
                                                   (gradient-descent names (optimize names values vars lr (- k 1) expr) vars lr expr) ) )

(optimize '(x y) '(10 20) '(x) 0.1 100 '(+ x y))
(optimize '(x y) '(10 20) '(x y) 1 5 '(+ x y))
(optimize '(x y) '(10 20) '(x) 0.01 1000 '(mse x y))
(optimize '(x y w1 w2 b) '(1 20 0.1 0.3 5) '(x) 0.001 1000 '(mse (+ (* x x w1) (* x w2) b) y))
