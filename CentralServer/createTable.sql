CREATE TABLE roles (
    id INTEGER PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(50) UNIQUE NOT NULL,           	-- 'Operator', 'Engineer', 'Admin'
    description VARCHAR(255),
    permissions TEXT,                            	-- JSON格式存储权限列表（TEXT可以）
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) UNIQUE NOT NULL,  			-- 改为VARCHAR，指定长度
    password_hash VARCHAR(255) NOT NULL,
    full_name VARCHAR(100),
    role varchar(20) NOT NULL,
    is_active BOOLEAN DEFAULT 1,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_login TIMESTAMP,
    created_by INTEGER,								-- 创建该用户的admin ID
    FOREIGN KEY (created_by) REFERENCES users(id),
    FOREIGN KEY (role) REFERENCES roles(name)
);

