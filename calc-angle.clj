(ns calc-angle.core
 (:require [clojure.string :as string])
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
  (println cos-beta)
  (list (- (/ PI 2.) (Math/acos cos-gamma)) (Math/acos cos-beta))))

(defn -main
 "main function"
 [& args]
 (let [alpha-beta (calc-alpha-beta 10 1)
       alpha (first alpha-beta)
       beta  (second alpha-beta)]
  (println (str "alpha,beta=" (string/join "," (map radian-to-degree alpha-beta))))
  (println (str "first-arm*cos(beta)=" (* first-arm (Math/cos beta))))
  (println (str "second-arm*cos(alpha)=" (* second-arm (Math/cos alpha))))
  (println (str "first-arm*sin(beta)=" (* first-arm (Math/sin beta))))
  (println (str "second-arm*sin(alpha)=" (* second-arm (Math/sin alpha))))))
