# 学习 php 扩展

[![CircleCI](https://circleci.com/gh/minbaby/php-ext-startup/tree/master.svg?style=svg)](https://circleci.com/gh/minbaby/php-ext-startup/tree/master)
[![xcxc](https://img.shields.io/github/license/minbaby/php-ext-startup.svg)](https://github.com/minbaby/php-ext-startup/blob/master/README.MD)
[![codecov](https://codecov.io/gh/minbaby/php-ext-startup/branch/master/graph/badge.svg)](https://codecov.io/gh/minbaby/php-ext-startup)

## 基本思路

1. 先写测试用例
2. 再用 php 实现
3. 再用 ext 实现
4. 跑测试

## 测试思路

利用 php [自动加载](http://www.php.net/manual/zh/language.oop5.autoload.php)的逻辑。

1. ~~默认禁用扩展执行一遍测试，这个时候执行的是 PHP 逻辑~~
2. ~~启用扩展再执行一次扩展，　这个时候执行的是 EXT 逻辑 (这个地方有个问题，如果 ext 扩展不存在，那么就会降级使用 php 代码)~~

使用函数动态生成对应的带有命名空间的类名，原因是之前的思路存在漏洞，因为自动加载的存在，导致如果 ext 中不存在，就会自动降级为 php 代码。

## 学习方式

如果我们需要查看这个项目，那么思路是什么的？

1. 先看 `spec/` 这些文件的测试用例。
2. 查看 `src/Minbaby/` 目录下的 php 实现
3. 查看 `src/ext` 目录下的 c 实现

## TODO

- [ ] [stringy](https://github.com/danielstjules/Stringy)

## 说明

- 使用 `@` 抑制符不是一个好习惯，尽量不要使用。
- 依赖 `mb_string` 扩展

## 已知问题

- 0.0

### skip ci

- `[skip ci]`
- `[ci skip]`

### 参考

- [phpbook](https://github.com/walu/phpbook)
- [Kahlan](https://kahlan.github.io/docs/index.html)
