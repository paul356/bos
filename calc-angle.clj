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

(defn -main
  "main function"
  [& args]
  (println "Hello, World!"))
