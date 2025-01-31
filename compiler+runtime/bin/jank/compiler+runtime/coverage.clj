#!/usr/bin/env bb

(ns jank.compiler+runtime.coverage
  (:require
   [babashka.fs :as b.f]
   [clojure.string]
   [jank.util :as util]))

(def compiler+runtime-dir (str (b.f/parent *file*) "/../../.."))

(defn -main [{:keys [enabled?]}]
  (util/log-step "Upload coverage report")
  (if-not enabled?
    (util/log-info "Not enabled")
    (util/with-elapsed-time duration
      (let [llvm-profdata (util/find-llvm-tool "llvm-profdata")
            llvm-cov (util/find-llvm-tool "llvm-cov")
            coverage-files (b.f/glob (str compiler+runtime-dir "/build")
                                     "jank-*.profraw"
                                     {:recursive false})
            merged-file (str compiler+runtime-dir "/build/jank.profdata")
            lcov-file (str compiler+runtime-dir "/build/jank.lcov")]
        (util/quiet-shell {:dir compiler+runtime-dir}
                          (str llvm-profdata " merge "
                               (clojure.string/join " " coverage-files)
                               " -o " merged-file))
        (util/quiet-shell {:dir compiler+runtime-dir
                           :out lcov-file}
                          (str llvm-cov " export --format=lcov --instr-profile " merged-file
                               " build/jank-test --object build/jank"))
        (let [codecov-script (str compiler+runtime-dir "/build/codecov")]
          (util/quiet-shell {:out codecov-script} "curl -s https://codecov.io/bash")
          (util/quiet-shell {}
                            (str "bash " codecov-script " -f " lcov-file))))
      (util/log-info-with-time duration "Merged and published coverage report"))))

(when (= *file* (System/getProperty "babashka.file"))
  (-main {:enabled? true}))
