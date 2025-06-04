#ifndef UUID134669146661776
#define UUID134669146661776

/**
  * RandomForestClassifier(bootstrap=True, ccp_alpha=0.0, class_name=RandomForestClassifier, class_weight=None, criterion=gini, estimator=DecisionTreeClassifier(), estimator_params=('criterion', 'max_depth', 'min_samples_split', 'min_samples_leaf', 'min_weight_fraction_leaf', 'max_features', 'max_leaf_nodes', 'min_impurity_decrease', 'random_state', 'ccp_alpha', 'monotonic_cst'), max_depth=None, max_features=sqrt, max_leaf_nodes=None, max_samples=None, min_impurity_decrease=0.0, min_samples_leaf=1, min_samples_split=2, min_weight_fraction_leaf=0.0, monotonic_cst=None, n_estimators=5, n_jobs=None, num_outputs=2, oob_score=False, package_name=everywhereml.sklearn.ensemble, random_state=None, template_folder=everywhereml/sklearn/ensemble, verbose=0, warm_start=False)
 */
class RandomForestClassifier {
    public:

        /**
         * Predict class from features
         */
        int predict(float *x) {
            int predictedValue = 0;
            size_t startedAt = micros();

            
                    
            float votes[2] = { 0 };
            uint8_t classIdx = 0;
            float classScore = 0;

            
                tree0(x, &classIdx, &classScore);
                votes[classIdx] += classScore;
            
                tree1(x, &classIdx, &classScore);
                votes[classIdx] += classScore;
            
                tree2(x, &classIdx, &classScore);
                votes[classIdx] += classScore;
            
                tree3(x, &classIdx, &classScore);
                votes[classIdx] += classScore;
            
                tree4(x, &classIdx, &classScore);
                votes[classIdx] += classScore;
            

            uint8_t maxClassIdx = 0;
            float maxVote = votes[0];

            for (uint8_t i = 1; i < 2; i++) {
                if (votes[i] > maxVote) {
                    maxClassIdx = i;
                    maxVote = votes[i];
                }
            }

            predictedValue = maxClassIdx;

                    

            latency = micros() - startedAt;

            return (lastPrediction = predictedValue);
        }

        
            
            /**
             * Get latency in micros
             */
            uint32_t latencyInMicros() {
                return latency;
            }

            /**
             * Get latency in millis
             */
            uint16_t latencyInMillis() {
                return latency / 1000;
            }
            

    protected:
        float latency = 0;
        int lastPrediction = 0;

        
            
        
            
                /**
                 * Random forest's tree #0
                 */
                void tree0(float *x, uint8_t *classIdx, float *classScore) {
                    
                        if (x[0] > 410.0) {
                            
                        if (x[2] < 142.8499984741211) {
                            
                        *classIdx = 0;
                        *classScore = 0.492;
                        return;

                        }
                        else {
                            
                        *classIdx = 1;
                        *classScore = 0.508;
                        return;

                        }

                        }
                        else {
                            
                        *classIdx = 0;
                        *classScore = 0.492;
                        return;

                        }

                }
            
        
            
                /**
                 * Random forest's tree #1
                 */
                void tree1(float *x, uint8_t *classIdx, float *classScore) {
                    
                        if (x[2] < 144.5500030517578) {
                            
                        *classIdx = 0;
                        *classScore = 0.492;
                        return;

                        }
                        else {
                            
                        if (x[1] < 150.6500015258789) {
                            
                        *classIdx = 1;
                        *classScore = 0.508;
                        return;

                        }
                        else {
                            
                        if (x[0] > 410.0) {
                            
                        *classIdx = 1;
                        *classScore = 0.508;
                        return;

                        }
                        else {
                            
                        *classIdx = 0;
                        *classScore = 0.492;
                        return;

                        }

                        }

                        }

                }
            
        
            
                /**
                 * Random forest's tree #2
                 */
                void tree2(float *x, uint8_t *classIdx, float *classScore) {
                    
                        if (x[2] < 142.8499984741211) {
                            
                        *classIdx = 0;
                        *classScore = 0.498;
                        return;

                        }
                        else {
                            
                        if (x[2] < 180.3000030517578) {
                            
                        if (x[2] < 179.9000015258789) {
                            
                        if (x[2] < 168.4000015258789) {
                            
                        if (x[2] < 163.6500015258789) {
                            
                        if (x[0] > 410.0) {
                            
                        *classIdx = 1;
                        *classScore = 0.502;
                        return;

                        }
                        else {
                            
                        *classIdx = 0;
                        *classScore = 0.498;
                        return;

                        }

                        }
                        else {
                            
                        if (x[0] > 410.0) {
                            
                        *classIdx = 1;
                        *classScore = 0.502;
                        return;

                        }
                        else {
                            
                        *classIdx = 0;
                        *classScore = 0.498;
                        return;

                        }

                        }

                        }
                        else {
                            
                        if (x[0] > 410.0) {
                            
                        *classIdx = 1;
                        *classScore = 0.502;
                        return;

                        }
                        else {
                            
                        *classIdx = 0;
                        *classScore = 0.498;
                        return;

                        }

                        }

                        }
                        else {
                            
                        if (x[0] > 391.5) {
                            
                        *classIdx = 1;
                        *classScore = 0.502;
                        return;

                        }
                        else {
                            
                        *classIdx = 0;
                        *classScore = 0.498;
                        return;

                        }

                        }

                        }
                        else {
                            
                        *classIdx = 1;
                        *classScore = 0.502;
                        return;

                        }

                        }

                }
            
        
            
                /**
                 * Random forest's tree #3
                 */
                void tree3(float *x, uint8_t *classIdx, float *classScore) {
                    
                        if (x[1] < 142.5) {
                            
                        *classIdx = 0;
                        *classScore = 0.51;
                        return;

                        }
                        else {
                            
                        if (x[1] < 149.9499969482422) {
                            
                        *classIdx = 1;
                        *classScore = 0.49;
                        return;

                        }
                        else {
                            
                        if (x[2] < 180.25) {
                            
                        if (x[1] < 150.6999969482422) {
                            
                        *classIdx = 0;
                        *classScore = 0.51;
                        return;

                        }
                        else {
                            
                        if (x[0] > 410.0) {
                            
                        *classIdx = 1;
                        *classScore = 0.49;
                        return;

                        }
                        else {
                            
                        *classIdx = 0;
                        *classScore = 0.51;
                        return;

                        }

                        }

                        }
                        else {
                            
                        *classIdx = 1;
                        *classScore = 0.49;
                        return;

                        }

                        }

                        }

                }
            
        
            
                /**
                 * Random forest's tree #4
                 */
                void tree4(float *x, uint8_t *classIdx, float *classScore) {
                    
                        if (x[2] < 142.8499984741211) {
                            
                        *classIdx = 0;
                        *classScore = 0.507;
                        return;

                        }
                        else {
                            
                        if (x[2] < 180.4000015258789) {
                            
                        if (x[1] < 149.9499969482422) {
                            
                        *classIdx = 1;
                        *classScore = 0.493;
                        return;

                        }
                        else {
                            
                        if (x[1] < 158.8499984741211) {
                            
                        if (x[1] < 157.5500030517578) {
                            
                        if (x[0] > 410.0) {
                            
                        *classIdx = 1;
                        *classScore = 0.493;
                        return;

                        }
                        else {
                            
                        *classIdx = 0;
                        *classScore = 0.507;
                        return;

                        }

                        }
                        else {
                            
                        if (x[1] < 157.6500015258789) {
                            
                        *classIdx = 0;
                        *classScore = 0.507;
                        return;

                        }
                        else {
                            
                        if (x[2] < 161.4499969482422) {
                            
                        *classIdx = 0;
                        *classScore = 0.507;
                        return;

                        }
                        else {
                            
                        if (x[0] > 404.0) {
                            
                        *classIdx = 1;
                        *classScore = 0.493;
                        return;

                        }
                        else {
                            
                        *classIdx = 0;
                        *classScore = 0.507;
                        return;

                        }

                        }

                        }

                        }

                        }
                        else {
                            
                        if (x[2] < 177.75) {
                            
                        if (x[2] < 162.75) {
                            
                        if (x[1] < 165.4499969482422) {
                            
                        if (x[0] > 492.0) {
                            
                        *classIdx = 1;
                        *classScore = 0.493;
                        return;

                        }
                        else {
                            
                        *classIdx = 0;
                        *classScore = 0.507;
                        return;

                        }

                        }
                        else {
                            
                        if (x[1] < 173.5999984741211) {
                            
                        *classIdx = 1;
                        *classScore = 0.493;
                        return;

                        }
                        else {
                            
                        if (x[0] > 446.0) {
                            
                        *classIdx = 1;
                        *classScore = 0.493;
                        return;

                        }
                        else {
                            
                        *classIdx = 0;
                        *classScore = 0.507;
                        return;

                        }

                        }

                        }

                        }
                        else {
                            
                        if (x[0] > 410.0) {
                            
                        *classIdx = 1;
                        *classScore = 0.493;
                        return;

                        }
                        else {
                            
                        *classIdx = 0;
                        *classScore = 0.507;
                        return;

                        }

                        }

                        }
                        else {
                            
                        if (x[1] < 180.5) {
                            
                        if (x[2] < 179.5999984741211) {
                            
                        if (x[0] > 416.0) {
                            
                        *classIdx = 1;
                        *classScore = 0.493;
                        return;

                        }
                        else {
                            
                        *classIdx = 0;
                        *classScore = 0.507;
                        return;

                        }

                        }
                        else {
                            
                        *classIdx = 0;
                        *classScore = 0.507;
                        return;

                        }

                        }
                        else {
                            
                        *classIdx = 1;
                        *classScore = 0.493;
                        return;

                        }

                        }

                        }

                        }

                        }
                        else {
                            
                        *classIdx = 1;
                        *classScore = 0.493;
                        return;

                        }

                        }

                }
            
        

            
};



static RandomForestClassifier fraudClassifier;


#endif