
/*
 * @Description: 
 * @Autor: C-Xingyu
 * @Date: 2021-11-09 22:20:50
 * @LastEditors: C-Xingyu
 * @LastEditTime: 2021-12-01 11:33:00
 */

#ifndef UKF_H
#define UKF_H

#include "/usr/include/eigen3/Eigen/Dense"
#include <vector>
#include "JPDA_UKF_Tracking/object_array.h"
#include "JPDA_UKF_Tracking/object.h"
const double PI = 3.14159;
enum TrackingState : int
{
    Die = 0,       // No longer tracking
    Init = 1,      // Start tracking
    Stable = 4,    // Stable tracking
    Occlusion = 5, // Lost 1 frame possibly by occlusion
    Lost = 10,     // About to lose target
};

enum Mode : int
{
    CV = 1,
    CTRV = 2,
    RM = 3,
};

class UKF
{
public:
    int tracking_num;
    int life_time;
    bool is_static;
    bool is_stable;

    int state_num_;
    int meas_num_;
    float alpha;
    float beta;
    float k;
    float lambda;

    std::vector<double> vel_history;
    JPDA_UKF_Tracking::object object_;
    Eigen::VectorXd mode_prob;
    Eigen::MatrixXd mode_trans_prob;
    Eigen::MatrixXd initial_mode_trans_prob;

    //过程噪声
    double std_a_cv_;
    double std_a_ctrv_;
    double std_a_rm_;

    double std_ctrv_yawdd_;
    double std_cv_yawdd_;
    double std_rm_yawdd_;
    //测量噪声
    double std_laspx_;
    double std_laspy_;

    Eigen::VectorXd w_s; //期望
    Eigen::VectorXd w_c; //方差

    Eigen::VectorXd X_merge; //状态量   x y v yaw dyaw
    Eigen::VectorXd X_cv;
    Eigen::VectorXd X_ctrv;
    Eigen::VectorXd X_rm;

    Eigen::MatrixXd P_merge; //状态量方差
    Eigen::MatrixXd P_cv;
    Eigen::MatrixXd P_ctrv;
    Eigen::MatrixXd P_rm;

    Eigen::MatrixXd Q_cv;
    Eigen::MatrixXd Q_ctrv;
    Eigen::MatrixXd Q_rm;

    Eigen::MatrixXd X_predict_sig_cv; //预测状态量
    Eigen::MatrixXd X_predict_sig_ctrv;
    Eigen::MatrixXd X_predict_sig_rm;

    Eigen::MatrixXd Z_sigma_cv; //
    Eigen::MatrixXd Z_sigma_ctrv;
    Eigen::MatrixXd Z_sigma_rm;

    Eigen::MatrixXd Z_predict_cv; //预测测量量
    Eigen::MatrixXd Z_predict_ctrv;
    Eigen::MatrixXd Z_predict_rm;

    Eigen::MatrixXd S_cv; //预测测量方差
    Eigen::MatrixXd S_ctrv;
    Eigen::MatrixXd S_rm;

    Eigen::MatrixXd R_cv;
    Eigen::MatrixXd R_ctrv;
    Eigen::MatrixXd R_rm;

    Eigen::MatrixXd C_xz_cv; //协方差
    Eigen::MatrixXd C_xz_ctrv;
    Eigen::MatrixXd C_xz_rm;

    Eigen::MatrixXd K_cv; //卡尔曼增益
    Eigen::MatrixXd K_ctrv;
    Eigen::MatrixXd K_rm;

public:
    UKF();
    ~UKF();
    void Initialize(Eigen::VectorXd &init_meas);
    double NormalizeAngle(double angle);
    void Predict(const double dt);
    void InitialCovQ(const double dt);   //初始化Q
    void Interaction();                  //计算交互矩阵
    void PredictMotion(const double dt); //预测各个模型的状态量
    void MixProbability();
    void PredictMotion(const double dt, int mode);
    void CV(const Eigen::MatrixXd &X_sig, const double dt);
    void CTRV(const Eigen::MatrixXd &X_sig, const double dt);
    void RM(const Eigen::MatrixXd &X_sig, const double dt);
    void PredictMeasurement(int mode); //预测测量量
    void Update(JPDA_UKF_Tracking::object_array matched_object,
                double gating_threshold, double detection_probability, double gate_probability);
    void UpdateMotion(JPDA_UKF_Tracking::object_array matched_object, std::vector<double> model_update_prob,
                      double gating_threshold, double detection_probability, double gate_probability);
    void CalculateKalmanGain();
    void FindMaxZMaxS(Eigen::VectorXd max_Z, Eigen::MatrixXd max_S);
    void UpdateModelProbability(std::vector<double> update_model_prob);
    void CombineModel();
    void UpdateYawWithHighProb();
};

#endif
