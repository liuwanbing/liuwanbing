
//
//  GGDirectSeedingVCtl.m
//  ThePeople
//
//  Created by aoyolo on 16/4/12.
//  Copyright © 2016年 Ms.Gao. All rights reserved.
//

#import "GGDirectSeedingVCtl.h"
#import "GGPlayViewCtl.h"
#import "GGMovieCell.h"
#import "GGMovieModel.h"
#import "MJRefresh.h"
@interface GGDirectSeedingVCtl ()<UICollectionViewDelegate,UICollectionViewDataSource,UICollectionViewDelegateFlowLayout>
@property (nonatomic , strong)UICollectionView *collectionView;
@property (nonatomic , strong)NSMutableArray *dataArr;
@property (nonatomic , strong)UICollectionViewFlowLayout *flowLayout;
@end

@implementation GGDirectSeedingVCtl

#pragma mark 懒加载存放数据的数组
-(NSMutableArray *)dataArr{
    if (_dataArr == nil) {
        _dataArr = [[NSMutableArray alloc]init];
    }
    return _dataArr;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self reloadCollectionView];
    [self registerCell];
    [self httpRequest];
    
    //添加下拉刷新控件
    [self.collectionView addHeaderWithTarget:self action:@selector(httpRequest)];
    //自动下拉刷新
    [self.collectionView headerBeginRefreshing];
   
}

#pragma mark 添加UIControllerView
-(void)reloadCollectionView{
    UICollectionViewFlowLayout *flowLayout = [[UICollectionViewFlowLayout alloc] init];
    UICollectionView *collectionView = [[UICollectionView alloc] initWithFrame:self.view.bounds collectionViewLayout:flowLayout];
    self.flowLayout = flowLayout;
    self.collectionView = collectionView;
    self.collectionView.delegate = self;
    self.collectionView.dataSource = self;
    [self.view addSubview:self.collectionView];
    self.collectionView.backgroundColor = [UIColor colorWithRed:240/255.0 green:240/255.0 blue:240/255.0 alpha:1];
    self.collectionView.showsVerticalScrollIndicator= NO;
}

#pragma make -注册cell
- (void)registerCell{
    [self.collectionView registerNib:[UINib nibWithNibName:@"GGMovieCell" bundle:nil] forCellWithReuseIdentifier:@"cell"];
}

#pragma mark -解析网络数据
- (void)httpRequest{
    AFHTTPRequestOperationManager *manager = [AFHTTPRequestOperationManager manager];
    
    [manager GET:DirectSeedingUrl parameters:nil success:^(AFHTTPRequestOperation * _Nonnull operation, id  _Nonnull responseObject) {
        if (self.dataArr.count) {
            [self.dataArr removeAllObjects];
        }
        for (NSDictionary *dic in responseObject[@"data"]) {
            [self.collectionView headerEndRefreshing];
            GGMovieModel *GameM = [[GGMovieModel alloc]init];
            GameM.avatar = dic[@"avatar"];
            GameM.thumb  = dic[@"thumb"];
            GameM.title  = dic[@"title"];
            GameM.nick   = dic[@"nick"];
            GameM.view   = dic[@"view"];
            GameM.uid    = dic[@"uid"];
            [self.dataArr addObject:GameM];
        }
        [self.collectionView reloadData];
        
    } failure:^(AFHTTPRequestOperation * _Nonnull operation, NSError * _Nonnull error) {
    }];
}

#pragma mark <UICollectionViewDataSource>
- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView {
    return 1;
}


- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return self.dataArr.count;
}

-(CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath{
    return CGSizeMake((ScreenWidth-15)/2, (ScreenWidth-15)/2-10);
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *cellID = @"cell";
    GGMovieCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:cellID forIndexPath:indexPath];
    [cell setCellModel:self.dataArr[indexPath.row]];
    return cell;
}

- (UIEdgeInsets)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout insetForSectionAtIndex:(NSInteger)section{
    return UIEdgeInsetsMake(5, 5,5 , 5);
}
-(CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout minimumInteritemSpacingForSectionAtIndex:(NSInteger)section{
    return 5;
}
-(CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout minimumLineSpacingForSectionAtIndex:(NSInteger)section{
    return 5;
}
#pragma mark <UICollectionViewDelegate>
- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath{
    GGMovieModel *model = self.dataArr[indexPath.row];
    GGPlayViewCtl *playCtl= [[GGPlayViewCtl alloc]initWithVideoId:model.uid];
    playCtl.hidesBottomBarWhenPushed = YES;
    [self.navigationController pushViewController:playCtl animated:NO];
}



- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

@end
