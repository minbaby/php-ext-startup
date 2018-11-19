# 学习 php 扩展

[TOC]

[![CircleCI](https://circleci.com/gh/minbaby/php-ext-startup/tree/master.svg?style=svg)](https://circleci.com/gh/minbaby/php-ext-startup/tree/master)

## 基本思路

1. 先写测试用例
2. 再用 php 实现
3. 再用 ext 实现
4. 跑测试

## 测试思路

利用 php [自动加载](http://www.php.net/manual/zh/language.oop5.autoload.php)的逻辑。

1. 默认禁用扩展执行一遍测试，这个时候执行的是 PHP 逻辑
2. 启用扩展再执行一次扩展，　这个时候执行的是 EXT 逻辑 (这个地方有个问题，如果 ext 扩展不存在，那么就会降级使用 php 代码)

## 学习方式

如果我们需要查看这个项目，那么思路是什么的？

1. 先看 `spec/` 这些文件的测试用例。
2. 查看 `src/Minbaby/` 目录下的 php 实现
3. 查看 `src/ext` 目录下的 c 实现

## TODO

- [ ] [stringy](https://github.com/danielstjules/Stringy)

## 说明

- 使用 `@` 抑制符不是一个好习惯，尽量不要使用。

## 已知问题

- 和 xdebug 不兼容，开启 xdebug 会出现崩溃现象，原因未知（能力有限 😅）

### skip ci

- `[skip ci]`
- `[ci skip]`

### 参考

- [phpbook](https://github.com/walu/phpbook)
- [Kahlan](https://kahlan.github.io/docs/index.html)
