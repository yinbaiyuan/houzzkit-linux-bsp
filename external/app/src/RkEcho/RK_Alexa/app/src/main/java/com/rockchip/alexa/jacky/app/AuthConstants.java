/**
 * Copyright 2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * You may not use this file except in compliance with the License. A copy of the License is located the "LICENSE.txt"
 * file accompanying this source. This file is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the specific language governing permissions and limitations
 * under the License.
 */
package com.rockchip.alexa.jacky.app;

public class AuthConstants {
    public static final String SESSION_ID = "sessionId";
    public static final String CODE_VERIFIER= "codeVerifier";

    public static final String CLIENT_ID = "clientId";
    public static final String REDIRECT_URI = "redirectUri";
    public static final String AUTH_CODE = "authCode";

    public static final String CODE_CHALLENGE = "codeChallenge";
    public static final String CODE_CHALLENGE_METHOD = "codeChallengeMethod";
    public static final String DSN = "dsn";
    public static final String PRODUCT_ID = "productId";

    public static final String ALEXA_ALL_SCOPE = "alexa:all";
    public static final String SCOPE_PROFILE = "profile";
    public static final String SCOPE_PROFILE_USERID = "profile:user_id";
    public static final String DEVICE_SERIAL_NUMBER = "deviceSerialNumber";
    public static final String PRODUCT_INSTANCE_ATTRIBUTES = "productInstanceAttributes";

    public static final String BUNDLE_KEY_EXCEPTION = "exception";

    public static final String KEY = "Rockchip_83991906";

    public static enum CommunicateType {
        SOCKET,
        HTTPS
    };

    public static CommunicateType COMMUNICATE_TYPE = CommunicateType.HTTPS;

    public static class DeviceProvisioning {
        public static final String PRODUCT_ID = "alexa_for_rockchip";
        public static final String DSN = "123456";
    }
}
