#include "mipmap.h"

float mipmap::getLevel(HitRecord hit, int w, int h) {
    glm::vec3 p_obj = hit.p_obj;
    glm::vec3 n_obj = hit.n_obj;
    glm::vec3 d = hit.d_world;

    glm::mat3 inv_ctm = hit.shape->i_ctm;
    PrimitiveType type = hit.shape->primitive.type;
    glm::vec3 n = hit.normal;
    float t0 = hit.root;


    // First term is dp_dx
    int num_samples = g_m_Config.samplesPerPixel;
    float subpixel_scale = 1.0f / std::sqrt((float)num_samples);
    glm::vec3 u_cam = Camera::u * (1.0f / (float)w) * subpixel_scale;
    glm::vec3 v_cam = Camera::v * (1.0f / (float)h) * subpixel_scale;

    glm::vec3 dd_dx = (glm::dot(d,d)*u_cam - glm::dot(d,u_cam)*d)/(float)(std::pow(glm::dot(d,d), 1.5f));
    glm::vec3 dd_dy = (glm::dot(d,d)*v_cam - glm::dot(d,v_cam)*d)/(float)(std::pow(glm::dot(d,d), 1.5f));

    float dt0_dx = -t0*glm::dot(n,dd_dx)/glm::dot(n,d);
    float dt0_dy = -t0*glm::dot(n,dd_dy)/glm::dot(n,d);

    glm::vec3 dp_dx = t0 *dd_dx + dt0_dx * d;
    glm::vec3 dp_dy = t0 *dd_dy + dt0_dy * d;


    // Second term is du_dp
    glm::vec3 du_dp(0.f,0.f,0.f);
    glm::vec3 dv_dp(0.f,0.f,0.f);
    switch (type) {
    case PrimitiveType::PRIMITIVE_SPHERE: {
        du_dp.x = 1.f/(2*M_PI) *p_obj.z/(p_obj.x*p_obj.x+p_obj.z*p_obj.z);
        du_dp.z = -1.f/(2*M_PI) *p_obj.x/(p_obj.x*p_obj.x+p_obj.z*p_obj.z);

        dv_dp.y = 1.f/(M_PI*sqrt(0.25f - p_obj.y*p_obj.y));
        break;
    }
    case PrimitiveType::PRIMITIVE_CYLINDER: {
        du_dp.x = 1.f/(2*M_PI) *p_obj.z/(p_obj.x*p_obj.x+p_obj.z*p_obj.z);
        du_dp.z = -1.f/(2*M_PI) *p_obj.x/(p_obj.x*p_obj.x+p_obj.z*p_obj.z);

        dv_dp.y = 1.f;
        break;
    }
    case PrimitiveType::PRIMITIVE_CONE: {
        du_dp.x = 1.f/(2*M_PI) *p_obj.z/(p_obj.x*p_obj.x+p_obj.z*p_obj.z);
        du_dp.z = -1.f/(2*M_PI) *p_obj.x/(p_obj.x*p_obj.x+p_obj.z*p_obj.z);

        dv_dp.y = 1.f;
        break;
    }
    case PrimitiveType::PRIMITIVE_CUBE: {
        glm::vec3 absN = glm::abs(n_obj);
        if (absN.x > absN.y && absN.x > absN.z) { du_dp = glm::vec3(0,0,-1); dv_dp = glm::vec3(0,1,0); }
        else if (absN.y > absN.x && absN.y > absN.z) { du_dp = glm::vec3(1,0,0); dv_dp = glm::vec3(0,0,-1); }
        else { du_dp = glm::vec3(1,0,0); dv_dp = glm::vec3(0,1,0); }
        break;
    }
    default:
        break;
    }


    // Third term is ds_du
    float R_u = hit.shape->primitive.material.textureMap.repeatU;
    float R_v = hit.shape->primitive.material.textureMap.repeatV;
    float ds_du = w*R_u;
    float dt_dv = h*R_v;


    //with M-1 to match object space
    dp_dx = inv_ctm * dp_dx;
    dp_dy = inv_ctm * dp_dy;

    float ds_dx = glm::dot(dp_dx, du_dp) * ds_du;
    float ds_dy = glm::dot(dp_dy,du_dp)* ds_du;
    float dt_dx = glm::dot(dp_dx,dv_dp)*dt_dv ;
    float dt_dy = glm::dot(dp_dy,dv_dp)*dt_dv ;

    float dx =1.f;
    float X = dx *sqrt(ds_dx*ds_dx +dt_dx*dt_dx);

    float dy =1.f;
    float Y = dy *sqrt(ds_dy*ds_dy +dt_dy*dt_dy);

    float S = glm::max(X,Y);
    return S;

}

