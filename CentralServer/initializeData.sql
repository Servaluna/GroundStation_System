INSERT INTO roles (name, description) VALUES
('Admin', '管理员：系统管理和用户管理'),
('Engineer', '工程师：管理升级包和技术审批'),
('Operator', '操作员：执行常规升级任务');

INSERT INTO users (username, password_hash, full_name, role, created_by)
VALUES (
    'root',
    '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92',
    '超级管理员',
    'Admin',
    1
);
UPDATE users SET created_by = id WHERE username = 'root';