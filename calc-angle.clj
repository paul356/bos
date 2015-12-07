(ns calc-angle.core
 (:gen-class))

(def PI 3.1415926)

(def first-arm 14.)

(defn get-first-angle [ang] 
 (+ ang (- 45. 21.36363132)))

(def second-arm 15.2)

(defn get-second-angle [ang] 
 (+ ang (- 60. 41.27344032)))

(defn normalize-angle [ang]
 (loop [a ang]
  (if (and (not (neg? a)) (< a (* 2 PI)))
   a
   (recur (+ a (if (neg? a) (* 2 PI) (* -2 PI)))))))

(defn calc-cos-beta [r z]
 (let [z2 (* z z)
  r2 (* r r)
  a (/ 
      (+ (- (* first-arm first-arm) (* second-arm second-arm)) z2 r2) 
      (* 2 first-arm))
  sqrt (Math/sqrt 
      (- 
       (+ z2 r2) 
       (* a a)))]
  (/ (- (* a r) (* z sqrt)) (+ z2 r2))))

(defn radian-to-degree [ang]
 (/ (* 180. ang) PI))

(defn calc-alpha-beta [r z]
 (let [cos-beta (calc-cos-beta r z)
       sin-beta (Math/sqrt (- 1 (* cos-beta cos-beta)))
       cos-gamma (/ (- (* first-arm sin-beta) z) second-arm)]
  (map radian-to-degree (list (- (/ PI 2.) (Math/acos cos-gamma)) (Math/acos cos-beta)))))

(defn -main
 "main function"
 [& args]
 (print (calc-alpha-beta 10 0)))
