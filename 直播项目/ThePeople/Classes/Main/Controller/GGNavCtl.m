//
//  GGNavCtl.m
//  ThePeopleTV
//
//  Created by aoyolo on 16/3/30.
//  Copyright © 2016年 高广. All rights reserved.
//

#import "GGNavCtl.h"

@interface GGNavCtl ()

@end

@implementation GGNavCtl

- (void)viewDidLoad {
    [super viewDidLoad];
}


#pragma mark 设置导航栏
- (void)pushViewController:(UIViewController *)viewController animated:(BOOL)animated{
    viewController.navigationItem.rightBarButtonItem = [UIBarButtonItem barButtonItemWithImage:@"Search"  highImage:@"Search_selected" target:self action:@selector(OnSearch) forControlEvents:UIControlEventTouchUpInside];
    [super pushViewController:viewController animated:animated];
}
-(void)OnSearch{
    [super popViewControllerAnimated:YES];
}
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

@end
